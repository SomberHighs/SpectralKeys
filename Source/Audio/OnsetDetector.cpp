#include "OnsetDetector.h"
#include "Utils/Constants.h"
#include <algorithm>
#include <cmath>

std::vector<double> OnsetDetector::detect(const std::vector<std::vector<float>>& magnitudeFrames,
                                          double sampleRate, int hopSize)
{
    spectralFlux.clear();
    std::vector<double> onsets;

    if (magnitudeFrames.size() < 2) return onsets;

    int numBins = (int)magnitudeFrames[0].size();

    // Compute spectral flux (half-wave rectified)
    for (size_t i = 1; i < magnitudeFrames.size(); ++i)
    {
        float flux = 0.0f;
        for (int k = 0; k < numBins; ++k)
        {
            float diff = magnitudeFrames[i][k] - magnitudeFrames[i - 1][k];
            if (diff > 0.0f)
                flux += diff;
        }
        spectralFlux.push_back(flux);
    }

    if (spectralFlux.empty()) return onsets;

    // Adaptive threshold with moving median
    int medianWindow = static_cast<int>(0.1 * sampleRate / hopSize);
    if (medianWindow < 3) medianWindow = 3;
    if (medianWindow % 2 == 0) medianWindow++;

    int halfWin = medianWindow / 2;
    float delta = 1.5f;
    float offset = 0.01f;

    double frameRate = sampleRate / hopSize;
    int minOnsetFrames = static_cast<int>(Constants::kMinOnsetIntervalSec * frameRate);

    int lastOnsetFrame = -minOnsetFrames;

    for (int i = 0; i < (int)spectralFlux.size(); ++i)
    {
        // Compute local median
        int start = std::max(0, i - halfWin);
        int end = std::min((int)spectralFlux.size(), i + halfWin + 1);
        std::vector<float> localWindow(spectralFlux.begin() + start, spectralFlux.begin() + end);
        std::sort(localWindow.begin(), localWindow.end());
        float median = localWindow[localWindow.size() / 2];

        float threshold = median * delta + offset;

        if (spectralFlux[i] > threshold && (i - lastOnsetFrame) >= minOnsetFrames)
        {
            double timeSec = (i + 1) * hopSize / sampleRate; // +1 because flux starts at frame 1
            onsets.push_back(timeSec);
            lastOnsetFrame = i;
        }
    }

    return onsets;
}
