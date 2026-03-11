#pragma once
#include <JuceHeader.h>
#include "Audio/PitchDetector.h"
#include <vector>

struct MidiNoteEvent
{
    int noteNumber = 60;
    float velocity = 0.8f;
    double startTime = 0.0;
    double endTime = 0.0;
};

class AudioToMidiConverter
{
public:
    AudioToMidiConverter() = default;

    std::vector<MidiNoteEvent> convert(const std::vector<PitchFrame>& pitchFrames,
                                        const std::vector<double>& onsets,
                                        const juce::AudioBuffer<float>& buffer,
                                        double sampleRate);

    juce::MidiMessageSequence toMidiSequence(const std::vector<MidiNoteEvent>& notes, double bpm);
};
