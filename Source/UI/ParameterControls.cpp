#include "ParameterControls.h"

ParameterControls::ParameterControls()
{
    setupSlider(onsetSensitivitySlider, 0.0, 1.0, 0.5, 0.01);
    setupSlider(noiseThresholdSlider, 0.0, 1.0, 0.2, 0.01);
    setupSlider(minNoteLengthSlider, 10.0, 500.0, 30.0, 1.0, " ms");

    setupLabel(onsetLabel);
    setupLabel(noiseLabel);
    setupLabel(minNoteLabel);
    setupLabel(gridLabel);

    // Quantize toggles
    auto setupToggle = [this](juce::ToggleButton& toggle) {
        toggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffaaaacc));
        toggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff4a9eff));
        toggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff3a3a5c));
        addAndMakeVisible(toggle);
        toggle.onClick = [this] { paramChanged(); };
    };
    setupToggle(quantizeKeyToggle);
    setupToggle(quantizeBpmToggle);

    // Grid subdivision
    gridSubdivisionBox.addItem("1/1", 1);
    gridSubdivisionBox.addItem("1/2", 2);
    gridSubdivisionBox.addItem("1/4", 4);
    gridSubdivisionBox.addItem("1/8", 8);
    gridSubdivisionBox.addItem("1/16", 16);
    gridSubdivisionBox.setSelectedId(4);
    gridSubdivisionBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a1a2e));
    gridSubdivisionBox.setColour(juce::ComboBox::textColourId, juce::Colour(0xffaaaacc));
    gridSubdivisionBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff3a3a5c));
    gridSubdivisionBox.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff4a9eff));
    addAndMakeVisible(gridSubdivisionBox);
    gridSubdivisionBox.onChange = [this] { paramChanged(); };

    addAndMakeVisible(gridLabel);
}

void ParameterControls::setupSlider(juce::Slider& slider, double min, double max,
                                     double defaultVal, double step, const juce::String& suffix)
{
    slider.setRange(min, max, step);
    slider.setValue(defaultVal, juce::dontSendNotification);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 55, 18);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1a1a30));
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4a9eff));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffddeeff));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffaaaacc));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff2a2a4a));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff12122a));
    addAndMakeVisible(slider);
    slider.onValueChange = [this] { paramChanged(); };
}

void ParameterControls::setupLabel(juce::Label& label)
{
    label.setColour(juce::Label::textColourId, juce::Colour(0xff666688));
    label.setFont(10.0f);
    label.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(label);
}

void ParameterControls::paramChanged()
{
    if (onParamsChanged) onParamsChanged();
}

AnalysisParams ParameterControls::getParams() const
{
    AnalysisParams p;
    p.onsetSensitivity = (float)onsetSensitivitySlider.getValue();
    p.noiseThreshold   = (float)noiseThresholdSlider.getValue();
    p.minNoteLengthMs  = (float)minNoteLengthSlider.getValue();
    p.quantizeToKey    = quantizeKeyToggle.getToggleState();
    p.quantizeToBpm    = quantizeBpmToggle.getToggleState();
    p.gridSubdivision  = gridSubdivisionBox.getSelectedId();
    if (p.gridSubdivision <= 0) p.gridSubdivision = 4;
    return p;
}

void ParameterControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    juce::ColourGradient gradient(juce::Colour(0xff12122a), bounds.getX(), bounds.getY(),
                                  juce::Colour(0xff161633), bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(juce::Colour(0xff2a2a4a));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    // Section title
    g.setColour(juce::Colour(0xff4a9eff));
    g.setFont(11.0f);
    g.drawText("CONTROLS", bounds.getX() + 8, 2, 80, 16, juce::Justification::centredLeft);
}

void ParameterControls::resized()
{
    auto bounds = getLocalBounds().reduced(6);
    bounds.removeFromTop(16); // title

    int labelWidth = 55;
    int rowHeight = 22;
    int gap = 3;

    // Left column: sliders
    auto leftCol = bounds.removeFromLeft(bounds.getWidth() / 2 - 4);

    // Onset sensitivity
    auto row = leftCol.removeFromTop(rowHeight);
    onsetLabel.setBounds(row.removeFromLeft(labelWidth));
    onsetSensitivitySlider.setBounds(row);
    leftCol.removeFromTop(gap);

    // Noise threshold
    row = leftCol.removeFromTop(rowHeight);
    noiseLabel.setBounds(row.removeFromLeft(labelWidth));
    noiseThresholdSlider.setBounds(row);
    leftCol.removeFromTop(gap);

    // Min note length
    row = leftCol.removeFromTop(rowHeight);
    minNoteLabel.setBounds(row.removeFromLeft(labelWidth));
    minNoteLengthSlider.setBounds(row);

    // Right column: toggles + grid
    bounds.removeFromLeft(8);
    auto rightCol = bounds;

    row = rightCol.removeFromTop(rowHeight);
    quantizeKeyToggle.setBounds(row);
    rightCol.removeFromTop(gap);

    row = rightCol.removeFromTop(rowHeight);
    quantizeBpmToggle.setBounds(row.removeFromLeft(row.getWidth() / 2));
    gridLabel.setBounds(row.removeFromLeft(35));
    gridSubdivisionBox.setBounds(row);
}
