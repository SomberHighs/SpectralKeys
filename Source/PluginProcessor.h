#pragma once
#include <JuceHeader.h>
#include "Audio/AudioFileManager.h"
#include "Audio/SpectrogramGenerator.h"
#include "Audio/PitchDetector.h"
#include "Audio/OnsetDetector.h"
#include "Audio/KeyDetector.h"
#include "Audio/BpmDetector.h"
#include "MIDI/AudioToMidiConverter.h"
#include "MIDI/MidiExporter.h"

class SpectralKeysProcessor : public juce::AudioProcessor,
                               public juce::Thread
{
public:
    SpectralKeysProcessor();
    ~SpectralKeysProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public API
    void loadAudioFile(const juce::File& file);
    bool isAnalysing() const { return isThreadRunning(); }

    // Analysis results
    AudioFileManager audioFileManager;
    SpectrogramGenerator spectrogramGenerator;
    std::vector<MidiNoteEvent> detectedNotes;
    juce::MidiMessageSequence midiSequence;
    KeyResult keyResult;
    float detectedBpm = 0.0f;
    std::atomic<bool> analysisComplete { false };
    MidiExporter midiExporter;

    // Playback
    std::atomic<bool> shouldPlay { false };
    std::atomic<int> playbackPosition { 0 };

private:
    void run() override; // Analysis thread

    juce::File pendingFile;
};
