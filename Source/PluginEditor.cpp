#include "PluginEditor.h"

SpectralKeysEditor::SpectralKeysEditor(SpectralKeysProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(820, 720);
    setResizable(true, true);
    setResizeLimits(650, 550, 1400, 1100);

    addAndMakeVisible(dropZone);
    addAndMakeVisible(spectrogram);
    addAndMakeVisible(infoDisplay);
    addAndMakeVisible(paramControls);
    addAndMakeVisible(pianoRoll);
    addAndMakeVisible(midiDrag);
    addAndMakeVisible(transport);

    dropZone.onFileDropped = [this](const juce::File& f) { handleFileDrop(f); };

    transport.onPlay = [this] {
        if (processor.audioFileManager.hasLoadedFile())
        {
            processor.midiSynth.stop();
            processor.shouldPlay = true;
            transport.setPlaying(true);
            transport.setMidiPlaying(false);
        }
    };

    transport.onPlayMidi = [this] {
        if (processor.analysisComplete && !processor.detectedNotes.empty())
        {
            processor.shouldPlay = false;
            processor.playbackPosition = 0;
            transport.setPlaying(false);
            processor.midiSynth.play();
            transport.setMidiPlaying(true);
        }
    };

    transport.onStop = [this] {
        processor.shouldPlay = false;
        processor.playbackPosition = 0;
        processor.midiSynth.stop();
        transport.setPlaying(false);
        transport.setMidiPlaying(false);
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

    paramControls.onParamsChanged = [this] { onParamsChanged(); };

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
    processor.currentParams = paramControls.getParams();
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

void SpectralKeysEditor::onParamsChanged()
{
    if (!processor.analysisComplete || processor.isAnalysing())
        return;

    auto params = paramControls.getParams();
    processor.reprocessMidi(params);
    pianoRoll.setNotes(processor.detectedNotes, processor.audioFileManager.getDurationSeconds());
    midiDrag.setEnabled(!processor.detectedNotes.empty());
}

void SpectralKeysEditor::timerCallback()
{
    if (wasAnalysing && processor.analysisComplete)
    {
        wasAnalysing = false;
        updateUIFromAnalysis();
    }

    if (processor.midiUpdated.exchange(false))
    {
        pianoRoll.setNotes(processor.detectedNotes, processor.audioFileManager.getDurationSeconds());
    }

    if (!processor.shouldPlay)
        transport.setPlaying(false);

    if (!processor.midiSynth.isPlaying())
        transport.setMidiPlaying(false);
}

void SpectralKeysEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient bgGradient(
        juce::Colour(0xff0a0a1e), 0.0f, 0.0f,
        juce::Colour(0xff141430), (float)getWidth(), (float)getHeight(), true);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Title
    g.setColour(juce::Colour(0xff4a9eff));
    g.setFont(juce::Font(22.0f).boldened());
    g.drawText("SpectralKeys", 12, 6, 200, 30, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff444466));
    g.setFont(11.0f);
    g.drawText("v1.1", getWidth() - 50, 10, 40, 20, juce::Justification::centredRight);
}

void SpectralKeysEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(36); // title

    dropZone.setBounds(bounds.removeFromTop(46));
    bounds.removeFromTop(5);

    spectrogram.setBounds(bounds.removeFromTop(bounds.getHeight() * 28 / 100));
    bounds.removeFromTop(5);

    infoDisplay.setBounds(bounds.removeFromTop(65));
    bounds.removeFromTop(5);

    paramControls.setBounds(bounds.removeFromTop(100));
    bounds.removeFromTop(5);

    pianoRoll.setBounds(bounds.removeFromTop(bounds.getHeight() - 44));
    bounds.removeFromTop(5);

    // Bottom bar
    auto bottomBar = bounds;
    midiDrag.setBounds(bottomBar.removeFromRight(120));
    bottomBar.removeFromRight(8);
    transport.setBounds(bottomBar);
}
