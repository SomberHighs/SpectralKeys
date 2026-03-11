#pragma once
#include <JuceHeader.h>

class InfoDisplayComponent : public juce::Component
{
public:
    InfoDisplayComponent();

    void paint(juce::Graphics& g) override;

    void setKey(const juce::String& key, float confidence);
    void setBpm(float bpm);
    void setDuration(double seconds);

private:
    juce::String detectedKey = "--";
    float keyConfidence = 0.0f;
    float detectedBpm = 0.0f;
    double duration = 0.0;

    void drawGlowingText(juce::Graphics& g, const juce::String& text,
                         juce::Rectangle<float> area, juce::Colour colour, float fontSize);
};
