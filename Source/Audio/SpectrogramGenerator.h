#pragma once
#include <JuceHeader.h>
#include <vector>

class SpectrogramGenerator
{
public:
    SpectrogramGenerator();

    void process(const juce::AudioBuffer<float>& buffer);
    const std::vector<std::vector<float>>& getSpectrogramData() const { return spectrogramDb; }
    int getNumFrames() const { return (int)spectrogramDb.size(); }
    int getNumBins() const { return numBins; }
    const std::vector<std::vector<float>>& getMagnitudeData() const { return spectrogramMag; }

private:
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    int numBins = 0;
    std::vector<std::vector<float>> spectrogramDb;
    std::vector<std::vector<float>> spectrogramMag;
};
