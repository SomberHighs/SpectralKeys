#pragma once
#include <vector>
#include <string>

struct KeyResult
{
    std::string keyName;
    int pitchClass = 0;      // 0=C, 1=C#, ... 11=B
    bool isMinor = false;
    float confidence = 0.0f;
};

class KeyDetector
{
public:
    KeyDetector() = default;

    KeyResult detect(const std::vector<std::vector<float>>& magnitudeFrames,
                     double sampleRate, int fftSize);

private:
    void computeChromagram(const std::vector<std::vector<float>>& magnitudeFrames,
                           double sampleRate, int fftSize,
                           std::vector<float>& chroma);

    float pearsonCorrelation(const std::vector<float>& a, const std::vector<float>& b);
};
