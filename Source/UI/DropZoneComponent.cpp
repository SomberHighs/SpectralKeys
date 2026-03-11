#include "DropZoneComponent.h"

DropZoneComponent::DropZoneComponent()
{
}

void DropZoneComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    float cornerSize = 8.0f;

    g.setColour(juce::Colour(0xff1a1a2e));
    g.fillRoundedRectangle(bounds, cornerSize);

    if (isDragOver)
        g.setColour(juce::Colour(0xff4a9eff));
    else
        g.setColour(juce::Colour(0xff3a3a5c));

    g.drawRoundedRectangle(bounds, cornerSize, 2.0f);

    if (isLoading)
    {
        g.setColour(juce::Colour(0xff4a9eff));
        g.setFont(16.0f);
        g.drawText("Analyzing...", bounds, juce::Justification::centred);
    }
    else if (fileName.isNotEmpty())
    {
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText(fileName, bounds, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xff888899));
        g.setFont(18.0f);
        g.drawText("Drop Audio File Here", bounds, juce::Justification::centred);
    }
}

bool DropZoneComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& f : files)
    {
        juce::File file(f);
        auto ext = file.getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".mp3" || ext == ".flac" ||
            ext == ".aiff" || ext == ".aif" || ext == ".ogg")
            return true;
    }
    return false;
}

void DropZoneComponent::fileDragEnter(const juce::StringArray&, int, int)
{
    isDragOver = true;
    repaint();
}

void DropZoneComponent::fileDragExit(const juce::StringArray&)
{
    isDragOver = false;
    repaint();
}

void DropZoneComponent::filesDropped(const juce::StringArray& files, int, int)
{
    isDragOver = false;
    repaint();

    if (!files.isEmpty() && onFileDropped)
        onFileDropped(juce::File(files[0]));
}
