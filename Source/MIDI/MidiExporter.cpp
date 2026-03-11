#include "MidiExporter.h"
#include "Utils/Constants.h"

juce::MidiFile MidiExporter::createMidiFile(const juce::MidiMessageSequence& sequence, double bpm)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(Constants::kTicksPerBeat);

    juce::MidiMessageSequence track;

    // Add tempo meta-event
    int microsecondsPerBeat = static_cast<int>(60000000.0 / bpm);
    auto tempoEvent = juce::MidiMessage::tempoMetaEvent(microsecondsPerBeat);
    track.addEvent(tempoEvent, 0.0);

    // Add all note events
    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        auto* event = sequence.getEventPointer(i);
        track.addEvent(event->message, event->message.getTimeStamp());
    }

    track.sort();
    track.updateMatchedPairs();
    midiFile.addTrack(track);

    return midiFile;
}

juce::File MidiExporter::exportToTempFile(const juce::MidiMessageSequence& sequence, double bpm)
{
    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
    auto tempFile = tempDir.getChildFile("SpectralKeys_output.mid");

    exportToFile(sequence, bpm, tempFile);
    return tempFile;
}

bool MidiExporter::exportToFile(const juce::MidiMessageSequence& sequence, double bpm, const juce::File& file)
{
    auto midiFile = createMidiFile(sequence, bpm);
    juce::FileOutputStream stream(file);

    if (!stream.openedOk())
        return false;

    stream.setPosition(0);
    stream.truncate();
    return midiFile.writeTo(stream);
}
