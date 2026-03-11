#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils/Constants.h"

SpectralKeysProcessor::SpectralKeysProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      Thread("SpectralKeys Analysis")
{
}

SpectralKeysProcessor::~SpectralKeysProcessor()
{
    stopThread(5000);
}

void SpectralKeysProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    hostSampleRate = sampleRate;
    midiSynth.prepareToPlay(sampleRate, samplesPerBlock);
}

void SpectralKeysProcessor::releaseResources()
{
}

void SpectralKeysProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Audio file playback
    if (shouldPlay && audioFileManager.hasLoadedFile())
    {
        const auto& audioBuffer = audioFileManager.getBuffer();
        int pos = playbackPosition.load();
        int numSamples = buffer.getNumSamples();
        int available = audioBuffer.getNumSamples() - pos;

        if (available > 0)
        {
            int toCopy = juce::jmin(numSamples, available);
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                buffer.copyFrom(ch, 0, audioBuffer, 0, pos, toCopy);

            playbackPosition.store(pos + toCopy);
        }
        else
        {
            shouldPlay = false;
            playbackPosition = 0;
        }
    }

    // MIDI synth playback (additive — mixes on top)
    if (midiSynth.isPlaying())
        midiSynth.renderBlock(buffer, buffer.getNumSamples());
}

void SpectralKeysProcessor::loadAudioFile(const juce::File& file)
{
    if (isThreadRunning())
        stopThread(5000);

    shouldPlay = false;
    midiSynth.stop();
    playbackPosition = 0;
    analysisComplete = false;
    pendingFile = file;
    startThread();
}

void SpectralKeysProcessor::run()
{
    if (!audioFileManager.loadFile(pendingFile))
        return;

    if (threadShouldExit()) return;

    // Run spectrogram
    spectrogramGenerator.process(audioFileManager.getBuffer());

    if (threadShouldExit()) return;

    // Detect key
    keyResult = KeyDetector().detect(
        spectrogramGenerator.getMagnitudeData(),
        Constants::kSampleRate,
        Constants::kFFTSize);

    if (threadShouldExit()) return;

    // Detect onsets with current sensitivity
    OnsetDetector onsetDetector;
    rawOnsets = onsetDetector.detect(
        spectrogramGenerator.getMagnitudeData(),
        Constants::kSampleRate,
        Constants::kHopSize,
        currentParams.onsetSensitivity);

    if (threadShouldExit()) return;

    BpmDetector bpmDetector;
    detectedBpm = bpmDetector.detect(
        onsetDetector.getSpectralFlux(),
        Constants::kSampleRate,
        Constants::kHopSize);

    if (threadShouldExit()) return;

    // Detect pitch
    PitchDetector pitchDetector;
    rawPitchFrames = pitchDetector.detectAll(
        audioFileManager.getBuffer(),
        Constants::kSampleRate);

    if (threadShouldExit()) return;

    // Convert to MIDI with current params
    AudioToMidiConverter converter;
    detectedNotes = converter.convert(
        rawPitchFrames, rawOnsets,
        audioFileManager.getBuffer(),
        Constants::kSampleRate);

    // Apply post-processing
    converter.applyNoiseFilter(detectedNotes, currentParams.noiseThreshold, currentParams.minNoteLengthMs);
    if (currentParams.quantizeToKey)
        converter.quantizeToKey(detectedNotes, keyResult);
    if (currentParams.quantizeToBpm)
        converter.quantizeToGrid(detectedNotes, detectedBpm, currentParams.gridSubdivision);

    midiSequence = converter.toMidiSequence(detectedNotes, detectedBpm);
    midiSynth.setNotes(detectedNotes);

    analysisComplete = true;
}

void SpectralKeysProcessor::reprocessMidi(const AnalysisParams& params)
{
    currentParams = params;

    if (!audioFileManager.hasLoadedFile() || rawPitchFrames.empty())
        return;

    // Re-run onset detection with new sensitivity
    OnsetDetector onsetDetector;
    rawOnsets = onsetDetector.detect(
        spectrogramGenerator.getMagnitudeData(),
        Constants::kSampleRate,
        Constants::kHopSize,
        params.onsetSensitivity);

    // Re-convert
    AudioToMidiConverter converter;
    detectedNotes = converter.convert(
        rawPitchFrames, rawOnsets,
        audioFileManager.getBuffer(),
        Constants::kSampleRate);

    // Apply post-processing
    converter.applyNoiseFilter(detectedNotes, params.noiseThreshold, params.minNoteLengthMs);
    if (params.quantizeToKey)
        converter.quantizeToKey(detectedNotes, keyResult);
    if (params.quantizeToBpm)
        converter.quantizeToGrid(detectedNotes, detectedBpm, params.gridSubdivision);

    midiSequence = converter.toMidiSequence(detectedNotes, detectedBpm);
    midiSynth.setNotes(detectedNotes);
    midiUpdated = true;
}

juce::AudioProcessorEditor* SpectralKeysProcessor::createEditor()
{
    return new SpectralKeysEditor(*this);
}

void SpectralKeysProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree state("SpectralKeysState");
    juce::MemoryOutputStream stream(destData, true);
    state.writeToStream(stream);
}

void SpectralKeysProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, sizeInBytes, false);
    juce::ValueTree::readFromStream(stream);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectralKeysProcessor();
}
