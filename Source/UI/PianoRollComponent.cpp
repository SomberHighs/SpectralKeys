#include "PianoRollComponent.h"
#include "Utils/Constants.h"

PianoRollComponent::PianoRollComponent()
{
}

juce::Colour PianoRollComponent::getNoteColour(int noteNumber)
{
    // Color by pitch class for a vibrant look
    int pc = noteNumber % 12;
    const juce::Colour colours[] = {
        juce::Colour(0xffff4466),  // C  - red
        juce::Colour(0xffff6644),  // C# - orange-red
        juce::Colour(0xffff9933),  // D  - orange
        juce::Colour(0xffffcc22),  // D# - gold
        juce::Colour(0xff88ee33),  // E  - lime
        juce::Colour(0xff33dd66),  // F  - green
        juce::Colour(0xff33ddaa),  // F# - teal
        juce::Colour(0xff33ccee),  // G  - cyan
        juce::Colour(0xff4499ff),  // G# - blue
        juce::Colour(0xff6655ff),  // A  - indigo
        juce::Colour(0xffaa44ff),  // A# - purple
        juce::Colour(0xffee44cc),  // B  - magenta
    };
    return colours[pc];
}

void PianoRollComponent::setNotes(const std::vector<MidiNoteEvent>& notes, double totalDuration)
{
    midiNotes = notes;
    duration = totalDuration;
    repaint();
}

void PianoRollComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colour(0xff0d0d1a));
    g.fillRect(bounds);

    if (midiNotes.empty() || duration <= 0.0)
    {
        g.setColour(juce::Colour(0xff3a3a5c));
        g.drawRect(bounds, 1.0f);
        g.setColour(juce::Colour(0xff555566));
        g.setFont(14.0f);
        g.drawText("Piano Roll", bounds, juce::Justification::centred);
        return;
    }

    // Find note range
    int minNote = 127, maxNote = 0;
    for (const auto& n : midiNotes)
    {
        minNote = juce::jmin(minNote, n.noteNumber);
        maxNote = juce::jmax(maxNote, n.noteNumber);
    }
    minNote = juce::jmax(0, minNote - 2);
    maxNote = juce::jmin(127, maxNote + 2);
    int noteRange = maxNote - minNote + 1;
    if (noteRange < 12) noteRange = 12;

    float noteHeight = bounds.getHeight() / noteRange;
    float pixelsPerSecond = bounds.getWidth() / (float)duration;

    // Draw grid lines at C notes
    g.setColour(juce::Colour(0xff1a1a30));
    for (int note = minNote; note <= maxNote; ++note)
    {
        if (note % 12 == 0)
        {
            float y = bounds.getBottom() - (note - minNote + 1) * noteHeight;
            g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
        }
    }

    // Draw notes with glow
    for (const auto& note : midiNotes)
    {
        float x = (float)(note.startTime * pixelsPerSecond);
        float w = (float)((note.endTime - note.startTime) * pixelsPerSecond);
        float y = bounds.getBottom() - (note.noteNumber - minNote + 1) * noteHeight;

        auto colour = getNoteColour(note.noteNumber);
        float alpha = juce::jlimit(0.5f, 1.0f, note.velocity);

        // Glow behind note
        g.setColour(colour.withAlpha(alpha * 0.2f));
        g.fillRoundedRectangle(x - 1, y - 1, w + 2, noteHeight + 2, 3.0f);

        // Note bar
        g.setColour(colour.withAlpha(alpha));
        g.fillRoundedRectangle(x, y, juce::jmax(2.0f, w), noteHeight - 1.0f, 2.0f);
    }

    // Border
    g.setColour(juce::Colour(0xff2a2a4a));
    g.drawRect(bounds, 1.0f);
}
