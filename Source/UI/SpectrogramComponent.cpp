#include "SpectrogramComponent.h"
#include "Utils/Constants.h"

SpectrogramComponent::SpectrogramComponent()
{
}

juce::Colour SpectrogramComponent::getColourForDb(float db)
{
    float normalized = juce::jlimit(0.0f, 1.0f,
        (db - Constants::kDbFloor) / (Constants::kDbCeiling - Constants::kDbFloor));

    // Black -> deep blue -> cyan -> yellow -> white
    if (normalized < 0.25f)
    {
        float t = normalized / 0.25f;
        return juce::Colour::fromFloatRGBA(0.0f, 0.0f, t * 0.6f, 1.0f);
    }
    else if (normalized < 0.5f)
    {
        float t = (normalized - 0.25f) / 0.25f;
        return juce::Colour::fromFloatRGBA(0.0f, t * 0.8f, 0.6f + t * 0.4f, 1.0f);
    }
    else if (normalized < 0.75f)
    {
        float t = (normalized - 0.5f) / 0.25f;
        return juce::Colour::fromFloatRGBA(t, 0.8f + t * 0.2f, 1.0f - t, 1.0f);
    }
    else
    {
        float t = (normalized - 0.75f) / 0.25f;
        return juce::Colour::fromFloatRGBA(1.0f, 1.0f, t, 1.0f);
    }
}

void SpectrogramComponent::setData(const std::vector<std::vector<float>>& spectrogramDb)
{
    if (spectrogramDb.empty())
    {
        hasData = false;
        return;
    }

    int numFrames = (int)spectrogramDb.size();
    int numBins = (int)spectrogramDb[0].size();

    // Only show up to ~8kHz (roughly half of typical bins)
    int displayBins = juce::jmin(numBins, numBins * 8000 / 22050);

    spectrogramImage = juce::Image(juce::Image::ARGB, numFrames, displayBins, true);

    for (int x = 0; x < numFrames; ++x)
    {
        for (int y = 0; y < displayBins; ++y)
        {
            int binIdx = y;
            float db = spectrogramDb[x][binIdx];
            spectrogramImage.setPixelAt(x, displayBins - 1 - y, getColourForDb(db));
        }
    }

    hasData = true;
    repaint();
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colour(0xff0d0d1a));
    g.fillRect(bounds);

    if (hasData)
    {
        g.drawImage(spectrogramImage, bounds,
                    juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        g.setColour(juce::Colour(0xff3a3a5c));
        g.drawRect(bounds, 1.0f);
        g.setColour(juce::Colour(0xff555566));
        g.setFont(14.0f);
        g.drawText("Spectrogram", bounds, juce::Justification::centred);
    }
}
