#include "MidiDragComponent.h"

MidiDragComponent::MidiDragComponent()
{
}

void MidiDragComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    if (isReady)
    {
        // Glowing button effect
        juce::ColourGradient gradient(juce::Colour(0xff1a3a5c), bounds.getCentreX(), bounds.getY(),
                                      juce::Colour(0xff0d2240), bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, 8.0f);

        g.setColour(juce::Colour(0xff4a9eff));
        g.drawRoundedRectangle(bounds, 8.0f, 2.0f);

        // Pulsing glow
        g.setColour(juce::Colour(0xff4a9eff).withAlpha(0.1f));
        g.fillRoundedRectangle(bounds.expanded(2.0f), 10.0f);

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText("Drag MIDI", bounds, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xff1a1a2e));
        g.fillRoundedRectangle(bounds, 8.0f);

        g.setColour(juce::Colour(0xff2a2a3e));
        g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

        g.setColour(juce::Colour(0xff555566));
        g.setFont(14.0f);
        g.drawText("Drag MIDI", bounds, juce::Justification::centred);
    }
}

void MidiDragComponent::mouseDown(const juce::MouseEvent&)
{
    isDragging = false;
}

void MidiDragComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (!isReady || !onDragStart) return;

    if (!isDragging && e.getDistanceFromDragStart() > 5)
    {
        isDragging = true;
        juce::File midiFile = onDragStart();

        if (midiFile.existsAsFile())
        {
            auto* container = findParentComponentOfClass<juce::DragAndDropContainer>();
            if (container != nullptr)
            {
                juce::StringArray files;
                files.add(midiFile.getFullPathName());
                container->performExternalDragDropOfFiles(files, false);
            }
        }
    }
}
