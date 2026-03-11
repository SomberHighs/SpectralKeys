#include "BpmDetector.h"
#include "Utils/Constants.h"
#include <cmath>
#include <algorithm>

float BpmDetector::detect(const std::vector<float>& spectralFlux, double sampleRate, int hopSize)
{
    if (spectralFlux.empty()) return 120.0f;

    double frameRate = sampleRate / hopSize;

    // Autocorrelation of onset strength signal
    int minLag = static_cast<int>(frameRate * 60.0 / Constants::kMaxBpm);
    int maxLag = static_cast<int>(frameRate * 60.0 / Constants::kMinBpm);
    maxLag = std::min(maxLag, (int)spectralFlux.size() - 1);

    if (minLag >= maxLag) return 120.0f;

    float bestCorr = -1.0f;
    int bestLag = minLag;

    for (int lag = minLag; lag <= maxLag; ++lag)
    {
        float corr = 0.0f;
        int count = (int)spectralFlux.size() - lag;

        for (int i = 0; i < count; ++i)
            corr += spectralFlux[i] * spectralFlux[i + lag];

        corr /= count;

        // Perceptual weighting: Gaussian centered at 120 BPM
        float bpmAtLag = static_cast<float>(60.0 * frameRate / lag);
        float weight = std::exp(-0.5f * std::pow((bpmAtLag - 120.0f) / 40.0f, 2.0f));
        corr *= weight;

        if (corr > bestCorr)
        {
            bestCorr = corr;
            bestLag = lag;
        }
    }

    float bpm = static_cast<float>(60.0 * frameRate / bestLag);

    // Round to 1 decimal
    return std::round(bpm * 10.0f) / 10.0f;
}
