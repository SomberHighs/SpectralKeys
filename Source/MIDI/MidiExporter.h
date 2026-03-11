#pragma once
#include <JuceHeader.h>

class MidiExporter
{
public:
    MidiExporter() = default;

    juce::File exportToTempFile(const juce::MidiMessageSequence& sequence, double bpm);
    bool exportToFile(const juce::MidiMessageSequence& sequence, double bpm, const juce::File& file);

private:
    juce::MidiFile createMidiFile(const juce::MidiMessageSequence& sequence, double bpm);
};
