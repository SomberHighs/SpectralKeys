cmake_minimum_required(VERSION 3.22)
project(SpectralKeys VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(SpectralKeys
    COMPANY_NAME            "SomberHighs"
    PLUGIN_MANUFACTURER_CODE Smbr
    PLUGIN_CODE              Spky
    FORMATS                  VST3 Standalone
    PRODUCT_NAME             "SpectralKeys"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        TRUE
    IS_MIDI_EFFECT           FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD  TRUE
)

target_sources(SpectralKeys
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Audio/AudioFileManager.cpp
        Source/Audio/SpectrogramGenerator.cpp
        Source/Audio/PitchDetector.cpp
        Source/Audio/OnsetDetector.cpp
        Source/Audio/KeyDetector.cpp
        Source/Audio/BpmDetector.cpp
        Source/MIDI/AudioToMidiConverter.cpp
        Source/MIDI/MidiExporter.cpp
        Source/UI/DropZoneComponent.cpp
        Source/UI/SpectrogramComponent.cpp
        Source/UI/InfoDisplayComponent.cpp
        Source/UI/PianoRollComponent.cpp
        Source/UI/MidiDragComponent.cpp
        Source/UI/TransportControls.cpp
)

target_include_directories(SpectralKeys PRIVATE Source)

target_link_libraries(SpectralKeys
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(SpectralKeys
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)
