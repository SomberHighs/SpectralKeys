#pragma once
#include <vector>

class OnsetDetector
{
public:
    OnsetDetector() = default;

    // Returns onset times in seconds
    std::vector<double> detect(const std::vector<std::vector<float>>& magnitudeFrames,
                               double sampleRate, int hopSize);

    // Returns the spectral flux signal (used by BpmDetector too)
    const std::vector<float>& getSpectralFlux() const { return spectralFlux; }

private:
    std::vector<float> spectralFlux;
};
