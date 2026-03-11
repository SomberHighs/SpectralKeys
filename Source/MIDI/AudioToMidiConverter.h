#pragma once
#include <JuceHeader.h>
#include "Audio/PitchDetector.h"
#include "Audio/KeyDetector.h"
#include "UI/ParameterControls.h"
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

    // Post-processing with parameters
    void applyNoiseFilter(std::vector<MidiNoteEvent>& notes, float threshold, float minNoteLengthMs);
    void quantizeToKey(std::vector<MidiNoteEvent>& notes, const KeyResult& key);
    void quantizeToGrid(std::vector<MidiNoteEvent>& notes, double bpm, int subdivision);

    juce::MidiMessageSequence toMidiSequence(const std::vector<MidiNoteEvent>& notes, double bpm);

private:
    static const bool scaleNotes[2][12]; // [major/minor][pitch class]
    int snapToScale(int noteNumber, int keyRoot, bool isMinor);
};
