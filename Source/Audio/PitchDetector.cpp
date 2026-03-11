#include "PitchDetector.h"
#include "Utils/Constants.h"

std::vector<PitchFrame> PitchDetector::detectAll(const juce::AudioBuffer<float>& buffer, double sampleRate)
{
    std::vector<PitchFrame> frames;
    const float* data = buffer.getReadPointer(0);
    int totalSamples = buffer.getNumSamples();

    for (int start = 0; start + Constants::kFFTSize <= totalSamples; start += Constants::kHopSize)
    {
        float confidence = 0.0f;
        float freq = detectPitch(data + start, Constants::kFFTSize, sampleRate, confidence);

        PitchFrame frame;
        frame.timeSeconds = start / sampleRate;
        frame.frequency = freq;
        frame.confidence = confidence;
        frame.voiced = (confidence > 0.5f && freq > 50.0f && freq < 4200.0f);

        frames.push_back(frame);
    }

    return frames;
}

float PitchDetector::detectPitch(const float* data, int numSamples, double sampleRate, float& confidence)
{
    // YIN algorithm
    int halfLen = numSamples / 2;
    std::vector<float> diff(halfLen, 0.0f);
    std::vector<float> cmndf(halfLen, 0.0f);

    // Step 1: Difference function
    for (int tau = 0; tau < halfLen; ++tau)
    {
        float sum = 0.0f;
        for (int j = 0; j < halfLen; ++j)
        {
            float delta = data[j] - data[j + tau];
            sum += delta * delta;
        }
        diff[tau] = sum;
    }

    // Step 2: Cumulative mean normalized difference
    cmndf[0] = 1.0f;
    float runningSum = 0.0f;

    for (int tau = 1; tau < halfLen; ++tau)
    {
        runningSum += diff[tau];
        cmndf[tau] = diff[tau] * tau / runningSum;
    }

    // Step 3: Absolute threshold
    int tauEstimate = -1;
    for (int tau = 2; tau < halfLen; ++tau)
    {
        if (cmndf[tau] < Constants::kYinThreshold)
        {
            while (tau + 1 < halfLen && cmndf[tau + 1] < cmndf[tau])
                ++tau;
            tauEstimate = tau;
            break;
        }
    }

    if (tauEstimate == -1)
    {
        confidence = 0.0f;
        return 0.0f;
    }

    // Step 4: Parabolic interpolation
    float betterTau = static_cast<float>(tauEstimate);
    if (tauEstimate > 0 && tauEstimate < halfLen - 1)
    {
        float s0 = cmndf[tauEstimate - 1];
        float s1 = cmndf[tauEstimate];
        float s2 = cmndf[tauEstimate + 1];
        float adjustment = (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0));
        if (std::isfinite(adjustment))
            betterTau -= adjustment;
    }

    confidence = 1.0f - cmndf[tauEstimate];
    return static_cast<float>(sampleRate / betterTau);
}
