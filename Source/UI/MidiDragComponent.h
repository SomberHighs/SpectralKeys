#pragma once
#include <JuceHeader.h>
#include <functional>

class MidiDragComponent : public juce::Component
{
public:
    MidiDragComponent();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    void setEnabled(bool enabled) { isReady = enabled; repaint(); }
    std::function<juce::File()> onDragStart;

private:
    bool isReady = false;
    bool isDragging = false;
};
