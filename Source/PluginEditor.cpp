#include "PluginEditor.h"

SpectralKeysEditor::SpectralKeysEditor(SpectralKeysProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(820, 620);
    setResizable(true, true);
    setResizeLimits(600, 450, 1400, 1000);

    addAndMakeVisible(dropZone);
    addAndMakeVisible(spectrogram);
    addAndMakeVisible(infoDisplay);
    addAndMakeVisible(pianoRoll);
    addAndMakeVisible(midiDrag);
    addAndMakeVisible(transport);

    dropZone.onFileDropped = [this](const juce::File& f) { handleFileDrop(f); };

    transport.onPlay = [this] {
        if (processor.audioFileManager.hasLoadedFile())
        {
            processor.shouldPlay = true;
            transport.setPlaying(true);
        }
    };

    transport.onStop = [this] {
        processor.shouldPlay = false;
        processor.playbackPosition = 0;
        transport.setPlaying(false);
    };

    transport.onBrowse = [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file",
            juce::File(),
            "*.wav;*.mp3;*.flac;*.aiff;*.aif;*.ogg");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode |
                                  juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc) {
                auto result = fc.getResult();
                if (result.existsAsFile())
                    handleFileDrop(result);
            });
    };

    midiDrag.onDragStart = [this]() -> juce::File {
        return processor.midiExporter.exportToTempFile(
            processor.midiSequence, processor.detectedBpm);
    };

    // If analysis already done (e.g. editor re-opened), update UI
    if (processor.analysisComplete)
        updateUIFromAnalysis();

    startTimerHz(30);
}

SpectralKeysEditor::~SpectralKeysEditor()
{
    stopTimer();
}

void SpectralKeysEditor::handleFileDrop(const juce::File& file)
{
    dropZone.setFileName(file.getFileNameWithoutExtension());
    dropZone.setLoading(true);
    midiDrag.setEnabled(false);
    processor.loadAudioFile(file);
    wasAnalysing = true;
}

void SpectralKeysEditor::updateUIFromAnalysis()
{
    spectrogram.setData(processor.spectrogramGenerator.getSpectrogramData());
    infoDisplay.setKey(juce::String(processor.keyResult.keyName), processor.keyResult.confidence);
    infoDisplay.setBpm(processor.detectedBpm);
    infoDisplay.setDuration(processor.audioFileManager.getDurationSeconds());
    pianoRoll.setNotes(processor.detectedNotes, processor.audioFileManager.getDurationSeconds());
    midiDrag.setEnabled(!processor.detectedNotes.empty());
    dropZone.setLoading(false);
}

void SpectralKeysEditor::timerCallback()
{
    if (wasAnalysing && processor.analysisComplete)
    {
        wasAnalysing = false;
        updateUIFromAnalysis();
    }

    if (!processor.shouldPlay)
        transport.setPlaying(false);
}

void SpectralKeysEditor::paint(juce::Graphics& g)
{
    // Full background gradient
    juce::ColourGradient bgGradient(
        juce::Colour(0xff0a0a1e), 0.0f, 0.0f,
        juce::Colour(0xff141430), (float)getWidth(), (float)getHeight(), true);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Title
    g.setColour(juce::Colour(0xff4a9eff));
    g.setFont(juce::Font(22.0f).boldened());
    g.drawText("SpectralKeys", 12, 6, 200, 30, juce::Justification::centredLeft);

    // Subtle version text
    g.setColour(juce::Colour(0xff444466));
    g.setFont(11.0f);
    g.drawText("v1.0", getWidth() - 50, 10, 40, 20, juce::Justification::centredRight);
}

void SpectralKeysEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(36); // title area

    dropZone.setBounds(bounds.removeFromTop(50));
    bounds.removeFromTop(6);

    spectrogram.setBounds(bounds.removeFromTop(bounds.getHeight() * 35 / 100));
    bounds.removeFromTop(6);

    infoDisplay.setBounds(bounds.removeFromTop(70));
    bounds.removeFromTop(6);

    pianoRoll.setBounds(bounds.removeFromTop(bounds.getHeight() - 46));
    bounds.removeFromTop(6);

    // Bottom bar: transport + midi drag
    auto bottomBar = bounds;
    midiDrag.setBounds(bottomBar.removeFromRight(120));
    bottomBar.removeFromRight(8);
    transport.setBounds(bottomBar);
}
