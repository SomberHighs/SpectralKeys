#pragma once
#include <JuceHeader.h>
#include <vector>

class SpectrogramComponent : public juce::Component
{
public:
    SpectrogramComponent();

    void paint(juce::Graphics& g) override;
    void setData(const std::vector<std::vector<float>>& spectrogramDb);

private:
    juce::Image spectrogramImage;
    bool hasData = false;

    juce::Colour getColourForDb(float db);
};
