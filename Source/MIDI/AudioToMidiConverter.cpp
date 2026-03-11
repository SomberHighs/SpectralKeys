#include "AudioToMidiConverter.h"
#include "Utils/Constants.h"
#include "Utils/MathUtils.h"
#include <algorithm>
#include <cmath>

std::vector<MidiNoteEvent> AudioToMidiConverter::convert(
    const std::vector<PitchFrame>& pitchFrames,
    const std::vector<double>& onsets,
    const juce::AudioBuffer<float>& buffer,
    double sampleRate)
{
    std::vector<MidiNoteEvent> notes;
    if (onsets.empty() || pitchFrames.empty()) return notes;

    double totalDuration = buffer.getNumSamples() / sampleRate;

    for (size_t i = 0; i < onsets.size(); ++i)
    {
        double segStart = onsets[i];
        double segEnd = (i + 1 < onsets.size()) ? onsets[i + 1] : totalDuration;

        // Find pitch frames within this segment
        std::vector<float> voicedFreqs;
        for (const auto& frame : pitchFrames)
        {
            if (frame.timeSeconds >= segStart && frame.timeSeconds < segEnd && frame.voiced)
                voicedFreqs.push_back(frame.frequency);
        }

        if (voicedFreqs.empty()) continue;

        // Take median frequency for the segment
        std::sort(voicedFreqs.begin(), voicedFreqs.end());
        float medianFreq = voicedFreqs[voicedFreqs.size() / 2];

        int midiNote = MathUtils::frequencyToMidiNoteRounded(medianFreq);
        if (midiNote < Constants::kMinMidiNote || midiNote > Constants::kMaxMidiNote)
            continue;

        // Compute velocity from RMS of segment
        int startSample = static_cast<int>(segStart * sampleRate);
        int endSample = static_cast<int>(segEnd * sampleRate);
        startSample = juce::jlimit(0, buffer.getNumSamples() - 1, startSample);
        endSample = juce::jlimit(startSample + 1, buffer.getNumSamples(), endSample);

        float rms = 0.0f;
        const float* data = buffer.getReadPointer(0);
        for (int s = startSample; s < endSample; ++s)
            rms += data[s] * data[s];
        rms = std::sqrt(rms / (endSample - startSample));

        float velocity = juce::jlimit(0.05f, 1.0f, rms * 4.0f);

        MidiNoteEvent note;
        note.noteNumber = midiNote;
        note.velocity = velocity;
        note.startTime = segStart;
        note.endTime = segEnd - 0.005; // small gap before next note
        notes.push_back(note);
    }

    return notes;
}

juce::MidiMessageSequence AudioToMidiConverter::toMidiSequence(const std::vector<MidiNoteEvent>& notes, double bpm)
{
    juce::MidiMessageSequence seq;
    double ticksPerSecond = Constants::kTicksPerBeat * bpm / 60.0;

    for (const auto& note : notes)
    {
        double startTick = note.startTime * ticksPerSecond;
        double endTick = note.endTime * ticksPerSecond;
        int vel = juce::jlimit(1, 127, static_cast<int>(note.velocity * 127.0f));

        seq.addEvent(juce::MidiMessage::noteOn(1, note.noteNumber, (juce::uint8)vel), startTick);
        seq.addEvent(juce::MidiMessage::noteOff(1, note.noteNumber, (juce::uint8)0), endTick);
    }

    seq.sort();
    seq.updateMatchedPairs();
    return seq;
}
