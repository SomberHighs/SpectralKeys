#pragma once
#include <JuceHeader.h>
#include "AudioToMidiConverter.h"
#include <vector>
#include <array>

class MidiSynthesizer
{
public:
    MidiSynthesizer();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void renderBlock(juce::AudioBuffer<float>& buffer, int numSamples);

    void setNotes(const std::vector<MidiNoteEvent>& notes) { midiNotes = notes; }
    void play() { playing = true; positionSeconds = 0.0; }
    void stop() { playing = false; positionSeconds = 0.0; activeVoices.fill({}); }
    bool isPlaying() const { return playing; }
    double getPosition() const { return positionSeconds; }

private:
    struct Voice
    {
        bool active = false;
        int noteNumber = 0;
        float velocity = 0.0f;
        double phase = 0.0;
        float envelope = 0.0f;
    };

    static constexpr int kMaxVoices = 16;
    std::array<Voice, kMaxVoices> activeVoices {};

    std::vector<MidiNoteEvent> midiNotes;
    double currentSampleRate = 44100.0;
    double positionSeconds = 0.0;
    bool playing = false;

    void noteOn(int noteNumber, float velocity);
    void noteOff(int noteNumber);
    float midiNoteToFreq(int note);
};
