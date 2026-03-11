#pragma once
#include <vector>

class BpmDetector
{
public:
    BpmDetector() = default;

    float detect(const std::vector<float>& spectralFlux, double sampleRate, int hopSize);
};
