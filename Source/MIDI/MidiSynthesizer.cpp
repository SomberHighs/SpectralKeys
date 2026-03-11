#include "MidiSynthesizer.h"
#include <cmath>

MidiSynthesizer::MidiSynthesizer()
{
}

void MidiSynthesizer::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    activeVoices.fill({});
}

float MidiSynthesizer::midiNoteToFreq(int note)
{
    return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f);
}

void MidiSynthesizer::noteOn(int noteNumber, float velocity)
{
    // Find free voice or steal quietest
    int slot = -1;
    float quietest = 999.0f;
    int quietestIdx = 0;

    for (int i = 0; i < kMaxVoices; ++i)
    {
        if (!activeVoices[i].active) { slot = i; break; }
        if (activeVoices[i].envelope < quietest)
        {
            quietest = activeVoices[i].envelope;
            quietestIdx = i;
        }
    }
    if (slot == -1) slot = quietestIdx;

    activeVoices[slot].active = true;
    activeVoices[slot].noteNumber = noteNumber;
    activeVoices[slot].velocity = velocity;
    activeVoices[slot].phase = 0.0;
    activeVoices[slot].envelope = 0.0f;
}

void MidiSynthesizer::noteOff(int noteNumber)
{
    for (auto& v : activeVoices)
    {
        if (v.active && v.noteNumber == noteNumber)
            v.active = false; // envelope will decay
    }
}

void MidiSynthesizer::renderBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    if (!playing || midiNotes.empty())
        return;

    float* left = buffer.getWritePointer(0);
    float* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int s = 0; s < numSamples; ++s)
    {
        double time = positionSeconds + s / currentSampleRate;

        // Trigger note-ons and note-offs
        for (const auto& note : midiNotes)
        {
            double prevTime = time - 1.0 / currentSampleRate;
            if (note.startTime > prevTime && note.startTime <= time)
                noteOn(note.noteNumber, note.velocity);
            if (note.endTime > prevTime && note.endTime <= time)
                noteOff(note.noteNumber);
        }

        // Render voices — soft saw wave with envelope
        float sample = 0.0f;
        for (auto& v : activeVoices)
        {
            if (v.envelope < 0.001f && !v.active)
                continue;

            // Envelope
            if (v.active)
                v.envelope = juce::jmin(1.0f, v.envelope + 0.002f); // attack
            else
                v.envelope *= 0.9995f; // release

            if (v.envelope < 0.0001f) continue;

            float freq = midiNoteToFreq(v.noteNumber);
            double phaseInc = freq / currentSampleRate;
            v.phase += phaseInc;
            if (v.phase >= 1.0) v.phase -= 1.0;

            // Polyblep-ish soft saw
            float saw = 2.0f * (float)v.phase - 1.0f;
            // Soften with a bit of sine blend
            float sine = std::sin((float)(v.phase * juce::MathConstants<double>::twoPi));
            float wave = saw * 0.6f + sine * 0.4f;

            sample += wave * v.envelope * v.velocity * 0.15f;
        }

        left[s] += sample;
        if (right) right[s] += sample;
    }

    positionSeconds += numSamples / currentSampleRate;

    // Check if past all notes
    double lastEnd = 0.0;
    for (const auto& n : midiNotes)
        if (n.endTime > lastEnd) lastEnd = n.endTime;

    if (positionSeconds > lastEnd + 0.5)
    {
        playing = false;
        positionSeconds = 0.0;
        activeVoices.fill({});
    }
}
