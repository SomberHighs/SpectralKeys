#pragma once
#include <JuceHeader.h>
#include <functional>

class TransportControls : public juce::Component
{
public:
    TransportControls();

    void paint(juce::Graphics& g) override;
    void resized() override;

    std::function<void()> onPlay;
    std::function<void()> onStop;
    std::function<void()> onBrowse;

    void setPlaying(bool playing) { isPlaying = playing; repaint(); }

private:
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton browseButton { "Browse" };

    bool isPlaying = false;
};
