#pragma once
#include <JuceHeader.h>
#include "MIDI/AudioToMidiConverter.h"
#include <vector>

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent();

    void paint(juce::Graphics& g) override;
    void setNotes(const std::vector<MidiNoteEvent>& notes, double totalDuration);

private:
    std::vector<MidiNoteEvent> midiNotes;
    double duration = 0.0;

    juce::Colour getNoteColour(int noteNumber);
};
