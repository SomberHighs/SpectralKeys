#include "AudioFileManager.h"
#include "Utils/Constants.h"

AudioFileManager::AudioFileManager()
{
    formatManager.registerBasicFormats();
}

bool AudioFileManager::loadFile(const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return false;

    fileSampleRate = reader->sampleRate;
    fileName = file.getFileNameWithoutExtension();

    juce::AudioBuffer<float> fileBuffer((int)reader->numChannels, (int)reader->lengthInSamples);
    reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);

    // Mix down to mono
    int numSamples = fileBuffer.getNumSamples();
    monoBuffer.setSize(1, numSamples);
    monoBuffer.clear();

    for (int ch = 0; ch < fileBuffer.getNumChannels(); ++ch)
        monoBuffer.addFrom(0, 0, fileBuffer, ch, 0, numSamples, 1.0f / fileBuffer.getNumChannels());

    // Resample to canonical sample rate if needed
    if (std::abs(fileSampleRate - Constants::kSampleRate) > 1.0)
    {
        double ratio = Constants::kSampleRate / fileSampleRate;
        int newNumSamples = static_cast<int>(numSamples * ratio);
        juce::AudioBuffer<float> resampledBuffer(1, newNumSamples);

        for (int i = 0; i < newNumSamples; ++i)
        {
            double srcIdx = i / ratio;
            int idx0 = static_cast<int>(srcIdx);
            int idx1 = juce::jmin(idx0 + 1, numSamples - 1);
            float frac = static_cast<float>(srcIdx - idx0);
            resampledBuffer.setSample(0, i,
                monoBuffer.getSample(0, idx0) * (1.0f - frac) +
                monoBuffer.getSample(0, idx1) * frac);
        }

        monoBuffer = std::move(resampledBuffer);
        fileSampleRate = Constants::kSampleRate;
    }

    loaded = true;
    return true;
}

double AudioFileManager::getDurationSeconds() const
{
    if (!loaded) return 0.0;
    return monoBuffer.getNumSamples() / Constants::kSampleRate;
}
