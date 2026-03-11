#include "KeyDetector.h"
#include "Utils/Constants.h"
#include <cmath>
#include <numeric>
#include <algorithm>

static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

// Krumhansl-Kessler key profiles
static const float majorProfile[] = { 6.35f, 2.23f, 3.48f, 2.33f, 4.38f, 4.09f, 2.52f, 5.19f, 2.39f, 3.66f, 2.29f, 2.88f };
static const float minorProfile[] = { 6.33f, 2.68f, 3.52f, 5.38f, 2.60f, 3.53f, 2.54f, 4.75f, 3.98f, 2.69f, 3.34f, 3.17f };

void KeyDetector::computeChromagram(const std::vector<std::vector<float>>& magnitudeFrames,
                                     double sampleRate, int fftSize,
                                     std::vector<float>& chroma)
{
    chroma.assign(12, 0.0f);

    if (magnitudeFrames.empty()) return;

    int numBins = (int)magnitudeFrames[0].size();
    float binResolution = static_cast<float>(sampleRate) / fftSize;

    for (const auto& frame : magnitudeFrames)
    {
        for (int k = 1; k < numBins; ++k)
        {
            float freq = k * binResolution;
            if (freq < 60.0f || freq > 4200.0f) continue;

            float midiNote = 69.0f + 12.0f * std::log2(freq / Constants::kA4Frequency);
            int pitchClass = ((int)std::round(midiNote)) % 12;
            if (pitchClass < 0) pitchClass += 12;

            chroma[pitchClass] += frame[k] * frame[k]; // energy
        }
    }

    // Normalize
    float maxVal = *std::max_element(chroma.begin(), chroma.end());
    if (maxVal > 0.0f)
    {
        for (auto& c : chroma)
            c /= maxVal;
    }
}

float KeyDetector::pearsonCorrelation(const std::vector<float>& a, const std::vector<float>& b)
{
    int n = (int)a.size();
    float meanA = std::accumulate(a.begin(), a.end(), 0.0f) / n;
    float meanB = std::accumulate(b.begin(), b.end(), 0.0f) / n;

    float num = 0.0f, denA = 0.0f, denB = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        float da = a[i] - meanA;
        float db = b[i] - meanB;
        num += da * db;
        denA += da * da;
        denB += db * db;
    }

    float den = std::sqrt(denA * denB);
    if (den < 1e-10f) return 0.0f;
    return num / den;
}

KeyResult KeyDetector::detect(const std::vector<std::vector<float>>& magnitudeFrames,
                               double sampleRate, int fftSize)
{
    std::vector<float> chroma;
    computeChromagram(magnitudeFrames, sampleRate, fftSize, chroma);

    KeyResult best;
    best.confidence = -2.0f;

    for (int shift = 0; shift < 12; ++shift)
    {
        // Rotate chroma to test each key
        std::vector<float> rotated(12);
        for (int i = 0; i < 12; ++i)
            rotated[i] = chroma[(i + shift) % 12];

        // Test major
        std::vector<float> majProf(majorProfile, majorProfile + 12);
        float corrMaj = pearsonCorrelation(rotated, majProf);
        if (corrMaj > best.confidence)
        {
            best.confidence = corrMaj;
            best.pitchClass = shift;
            best.isMinor = false;
            best.keyName = std::string(noteNames[shift]) + " Major";
        }

        // Test minor
        std::vector<float> minProf(minorProfile, minorProfile + 12);
        float corrMin = pearsonCorrelation(rotated, minProf);
        if (corrMin > best.confidence)
        {
            best.confidence = corrMin;
            best.pitchClass = shift;
            best.isMinor = true;
            best.keyName = std::string(noteNames[shift]) + " Minor";
        }
    }

    return best;
}
