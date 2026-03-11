#include "AudioToMidiConverter.h"
#include "Utils/Constants.h"
#include "Utils/MathUtils.h"
#include <algorithm>
#include <cmath>

// Scale intervals: which pitch classes belong to major / minor
// Major: W W H W W W H  =>  0 2 4 5 7 9 11
// Minor: W H W W H W W  =>  0 2 3 5 7 8 10
const bool AudioToMidiConverter::scaleNotes[2][12] = {
    { true, false, true, false, true, true, false, true, false, true, false, true },  // major
    { true, false, true, true, false, true, false, true, true, false, true, false }   // minor
};

int AudioToMidiConverter::snapToScale(int noteNumber, int keyRoot, bool isMinor)
{
    int scaleIdx = isMinor ? 1 : 0;
    int pc = ((noteNumber - keyRoot) % 12 + 12) % 12;

    if (scaleNotes[scaleIdx][pc])
        return noteNumber; // already in scale

    // Try one semitone down, then up
    int down = ((pc - 1) % 12 + 12) % 12;
    int up = (pc + 1) % 12;

    if (scaleNotes[scaleIdx][down])
        return noteNumber - 1;
    if (scaleNotes[scaleIdx][up])
        return noteNumber + 1;

    // Fallback: two semitones
    return noteNumber;
}

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

        std::vector<float> voicedFreqs;
        for (const auto& frame : pitchFrames)
        {
            if (frame.timeSeconds >= segStart && frame.timeSeconds < segEnd && frame.voiced)
                voicedFreqs.push_back(frame.frequency);
        }

        if (voicedFreqs.empty()) continue;

        std::sort(voicedFreqs.begin(), voicedFreqs.end());
        float medianFreq = voicedFreqs[voicedFreqs.size() / 2];

        int midiNote = MathUtils::frequencyToMidiNoteRounded(medianFreq);
        if (midiNote < Constants::kMinMidiNote || midiNote > Constants::kMaxMidiNote)
            continue;

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
        note.endTime = segEnd - 0.005;
        notes.push_back(note);
    }

    return notes;
}

void AudioToMidiConverter::applyNoiseFilter(std::vector<MidiNoteEvent>& notes,
                                             float threshold, float minNoteLengthMs)
{
    float minDurationSec = minNoteLengthMs / 1000.0f;

    // threshold 0..1 maps to velocity cutoff 0..0.6
    float velCutoff = threshold * 0.6f;

    notes.erase(
        std::remove_if(notes.begin(), notes.end(),
            [velCutoff, minDurationSec](const MidiNoteEvent& n) {
                double dur = n.endTime - n.startTime;
                return n.velocity < velCutoff || dur < minDurationSec;
            }),
        notes.end());
}

void AudioToMidiConverter::quantizeToKey(std::vector<MidiNoteEvent>& notes, const KeyResult& key)
{
    for (auto& note : notes)
        note.noteNumber = snapToScale(note.noteNumber, key.pitchClass, key.isMinor);
}

void AudioToMidiConverter::quantizeToGrid(std::vector<MidiNoteEvent>& notes, double bpm, int subdivision)
{
    if (bpm <= 0.0 || subdivision <= 0) return;

    double beatDuration = 60.0 / bpm;
    double gridSize = beatDuration / (subdivision / 4.0); // subdivision relative to quarter note

    for (auto& note : notes)
    {
        // Snap start time to nearest grid line
        double gridIndex = std::round(note.startTime / gridSize);
        double snappedStart = gridIndex * gridSize;

        double shift = snappedStart - note.startTime;
        note.startTime = snappedStart;
        note.endTime += shift;

        // Ensure minimum duration of one grid unit
        if (note.endTime <= note.startTime + 0.001)
            note.endTime = note.startTime + gridSize;

        // Snap end time to grid too
        double endGrid = std::round(note.endTime / gridSize);
        note.endTime = endGrid * gridSize;
        if (note.endTime <= note.startTime)
            note.endTime = note.startTime + gridSize;

        // Small gap before next note
        note.endTime -= 0.002;
    }
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
