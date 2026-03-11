#include "TransportControls.h"

TransportControls::TransportControls()
{
    auto setupButton = [](juce::TextButton& btn, juce::Colour bg, juce::Colour text) {
        btn.setColour(juce::TextButton::buttonColourId, bg);
        btn.setColour(juce::TextButton::textColourOffId, text);
        btn.setColour(juce::TextButton::buttonOnColourId, bg.brighter(0.2f));
    };

    setupButton(playButton,     juce::Colour(0xff2a5a3a), juce::Colour(0xff4af0c0));
    setupButton(playMidiButton, juce::Colour(0xff2a3a5a), juce::Colour(0xff6ab0ff));
    setupButton(stopButton,     juce::Colour(0xff5a2a2a), juce::Colour(0xffff6666));
    setupButton(browseButton,   juce::Colour(0xff2a2a5a), juce::Colour(0xff9999ff));

    addAndMakeVisible(playButton);
    addAndMakeVisible(playMidiButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(browseButton);

    playButton.onClick     = [this] { if (onPlay) onPlay(); };
    playMidiButton.onClick = [this] { if (onPlayMidi) onPlayMidi(); };
    stopButton.onClick     = [this] { if (onStop) onStop(); };
    browseButton.onClick   = [this] { if (onBrowse) onBrowse(); };
}

void TransportControls::paint(juce::Graphics&)
{
}

void TransportControls::resized()
{
    auto bounds = getLocalBounds().reduced(4);
    int numButtons = 4;
    int gap = 5;
    int buttonWidth = (bounds.getWidth() - gap * (numButtons - 1)) / numButtons;

    playButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(gap);
    playMidiButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(gap);
    stopButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(gap);
    browseButton.setBounds(bounds);
}
