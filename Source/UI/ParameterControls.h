#pragma once
#include <JuceHeader.h>
#include <functional>

struct AnalysisParams
{
    float onsetSensitivity = 0.5f;   // 0.0 = few slices, 1.0 = many slices
    float noiseThreshold   = 0.2f;   // 0.0 = keep everything, 1.0 = only loud/long notes
    float minNoteLengthMs  = 30.0f;  // minimum note duration in ms
    bool  quantizeToKey    = false;
    bool  quantizeToBpm    = false;
    int   gridSubdivision  = 4;      // 1=whole, 2=half, 4=quarter, 8=eighth, 16=sixteenth
};

class ParameterControls : public juce::Component
{
public:
    ParameterControls();

    void paint(juce::Graphics& g) override;
    void resized() override;

    AnalysisParams getParams() const;
    std::function<void()> onParamsChanged;

private:
    // Sliders
    juce::Slider onsetSensitivitySlider;
    juce::Slider noiseThresholdSlider;
    juce::Slider minNoteLengthSlider;

    // Toggles
    juce::ToggleButton quantizeKeyToggle { "Snap to Key" };
    juce::ToggleButton quantizeBpmToggle { "Snap to Grid" };

    // Grid subdivision combo
    juce::ComboBox gridSubdivisionBox;

    // Labels
    juce::Label onsetLabel     { {}, "SLICE" };
    juce::Label noiseLabel     { {}, "NOISE" };
    juce::Label minNoteLabel   { {}, "MIN NOTE" };
    juce::Label gridLabel      { {}, "GRID" };

    void setupSlider(juce::Slider& slider, double min, double max, double defaultVal,
                     double step, const juce::String& suffix = "");
    void setupLabel(juce::Label& label);
    void paramChanged();
};
