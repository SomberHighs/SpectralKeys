#pragma once
#include <JuceHeader.h>
#include <vector>

struct PitchFrame
{
    float frequency = 0.0f;
    float confidence = 0.0f;
    double timeSeconds = 0.0;
    bool voiced = false;
};

class PitchDetector
{
public:
    PitchDetector() = default;

    std::vector<PitchFrame> detectAll(const juce::AudioBuffer<float>& buffer, double sampleRate);

private:
    float detectPitch(const float* data, int numSamples, double sampleRate, float& confidence);
};
