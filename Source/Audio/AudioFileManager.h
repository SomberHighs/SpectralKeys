#pragma once
#include <JuceHeader.h>

class AudioFileManager
{
public:
    AudioFileManager();

    bool loadFile(const juce::File& file);
    const juce::AudioBuffer<float>& getBuffer() const { return monoBuffer; }
    double getFileSampleRate() const { return fileSampleRate; }
    double getDurationSeconds() const;
    int getNumSamples() const { return monoBuffer.getNumSamples(); }
    bool hasLoadedFile() const { return loaded; }
    juce::String getFileName() const { return fileName; }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> monoBuffer;
    double fileSampleRate = 44100.0;
    bool loaded = false;
    juce::String fileName;
};
