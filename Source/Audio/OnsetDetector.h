#pragma once
#include <vector>

class OnsetDetector
{
public:
    OnsetDetector() = default;

    // sensitivity: 0.0 = few slices, 1.0 = many slices
    std::vector<double> detect(const std::vector<std::vector<float>>& magnitudeFrames,
                               double sampleRate, int hopSize, float sensitivity = 0.5f);

    const std::vector<float>& getSpectralFlux() const { return spectralFlux; }

private:
    std::vector<float> spectralFlux;
};
