#include "InfoDisplayComponent.h"

InfoDisplayComponent::InfoDisplayComponent()
{
}

void InfoDisplayComponent::drawGlowingText(juce::Graphics& g, const juce::String& text,
                                            juce::Rectangle<float> area, juce::Colour colour, float fontSize)
{
    // Glow effect
    g.setColour(colour.withAlpha(0.15f));
    g.setFont(fontSize + 2.0f);
    g.drawText(text, area.translated(0, 1), juce::Justification::centred);

    g.setColour(colour.withAlpha(0.3f));
    g.setFont(fontSize + 1.0f);
    g.drawText(text, area.translated(0, 0.5f), juce::Justification::centred);

    // Main text
    g.setColour(colour);
    g.setFont(fontSize);
    g.drawText(text, area, juce::Justification::centred);
}

void InfoDisplayComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // Background with subtle gradient
    juce::ColourGradient gradient(juce::Colour(0xff12122a), bounds.getX(), bounds.getY(),
                                  juce::Colour(0xff1a1a35), bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(juce::Colour(0xff2a2a4a));
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

    float thirdWidth = bounds.getWidth() / 3.0f;

    // Key display
    auto keyArea = bounds.removeFromLeft(thirdWidth);
    g.setColour(juce::Colour(0xff666688));
    g.setFont(11.0f);
    g.drawText("KEY", keyArea.removeFromTop(20.0f), juce::Justification::centred);
    drawGlowingText(g, detectedKey, keyArea, juce::Colour(0xff4af0c0), 26.0f);

    // BPM display
    auto bpmArea = bounds.removeFromLeft(thirdWidth);
    g.setColour(juce::Colour(0xff666688));
    g.setFont(11.0f);
    g.drawText("BPM", bpmArea.removeFromTop(20.0f), juce::Justification::centred);
    juce::String bpmText = detectedBpm > 0 ? juce::String(detectedBpm, 1) : "--";
    drawGlowingText(g, bpmText, bpmArea, juce::Colour(0xff4a9eff), 26.0f);

    // Duration display
    g.setColour(juce::Colour(0xff666688));
    g.setFont(11.0f);
    g.drawText("DURATION", bounds.removeFromTop(20.0f), juce::Justification::centred);
    int mins = (int)(duration / 60.0);
    int secs = (int)duration % 60;
    juce::String durText = duration > 0 ? juce::String::formatted("%d:%02d", mins, secs) : "--:--";
    drawGlowingText(g, durText, bounds, juce::Colour(0xffcf6aff), 26.0f);
}

void InfoDisplayComponent::setKey(const juce::String& key, float confidence)
{
    detectedKey = key;
    keyConfidence = confidence;
    repaint();
}

void InfoDisplayComponent::setBpm(float bpm)
{
    detectedBpm = bpm;
    repaint();
}

void InfoDisplayComponent::setDuration(double seconds)
{
    duration = seconds;
    repaint();
}
