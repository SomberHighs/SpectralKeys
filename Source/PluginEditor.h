#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/DropZoneComponent.h"
#include "UI/SpectrogramComponent.h"
#include "UI/InfoDisplayComponent.h"
#include "UI/PianoRollComponent.h"
#include "UI/MidiDragComponent.h"
#include "UI/TransportControls.h"
#include "UI/ParameterControls.h"

class SpectralKeysEditor : public juce::AudioProcessorEditor,
                            public juce::DragAndDropContainer,
                            public juce::Timer
{
public:
    SpectralKeysEditor(SpectralKeysProcessor&);
    ~SpectralKeysEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SpectralKeysProcessor& processor;

    DropZoneComponent dropZone;
    SpectrogramComponent spectrogram;
    InfoDisplayComponent infoDisplay;
    PianoRollComponent pianoRoll;
    MidiDragComponent midiDrag;
    TransportControls transport;
    ParameterControls paramControls;

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool wasAnalysing = false;

    void handleFileDrop(const juce::File& file);
    void updateUIFromAnalysis();
    void onParamsChanged();
};
