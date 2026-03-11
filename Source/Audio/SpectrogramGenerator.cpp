#include "SpectrogramGenerator.h"
#include "Utils/Constants.h"
#include "Utils/MathUtils.h"

SpectrogramGenerator::SpectrogramGenerator()
    : fft(Constants::kFFTOrder),
      window(Constants::kFFTSize, juce::dsp::WindowingFunction<float>::hann)
{
    numBins = Constants::kFFTSize / 2;
}

void SpectrogramGenerator::process(const juce::AudioBuffer<float>& buffer)
{
    spectrogramDb.clear();
    spectrogramMag.clear();

    const float* data = buffer.getReadPointer(0);
    int totalSamples = buffer.getNumSamples();

    std::vector<float> fftBuffer(Constants::kFFTSize * 2, 0.0f);

    for (int start = 0; start + Constants::kFFTSize <= totalSamples; start += Constants::kHopSize)
    {
        std::copy(data + start, data + start + Constants::kFFTSize, fftBuffer.begin());
        std::fill(fftBuffer.begin() + Constants::kFFTSize, fftBuffer.end(), 0.0f);

        window.multiplyWithWindowingTable(fftBuffer.data(), Constants::kFFTSize);
        fft.performFrequencyOnlyForwardTransform(fftBuffer.data());

        std::vector<float> magnitudes(numBins);
        std::vector<float> dbValues(numBins);

        for (int i = 0; i < numBins; ++i)
        {
            magnitudes[i] = fftBuffer[i];
            dbValues[i] = MathUtils::amplitudeToDb(fftBuffer[i]);
        }

        spectrogramMag.push_back(std::move(magnitudes));
        spectrogramDb.push_back(std::move(dbValues));
    }
}
