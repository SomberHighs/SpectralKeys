#pragma once
#include <JuceHeader.h>
#include <functional>

class DropZoneComponent : public juce::Component,
                          public juce::FileDragAndDropTarget
{
public:
    DropZoneComponent();

    void paint(juce::Graphics& g) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    std::function<void(const juce::File&)> onFileDropped;

    void setFileName(const juce::String& name) { fileName = name; repaint(); }
    void setLoading(bool loading) { isLoading = loading; repaint(); }

private:
    bool isDragOver = false;
    bool isLoading = false;
    juce::String fileName;
};
