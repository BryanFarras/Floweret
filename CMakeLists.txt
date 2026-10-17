cmake_minimum_required(VERSION 3.22)
project(Floweret VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate JUCE
if(NOT TARGET juce::juce_core)
    if(EXISTS "C:/Users/asus/Documents/JUCE/CMakeLists.txt")
        add_subdirectory("C:/Users/asus/Documents/JUCE" JUCE)
    else()
        find_package(JUCE CONFIG REQUIRED)
    endif()
endif()

# Create JUCE Audio Plugin (VST3 + Standalone)
juce_add_plugin(Floweret
    COMPANY_NAME "FloweretAudio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Flwa
    PLUGIN_CODE Flwt
    FORMATS VST3 Standalone
    PRODUCT_NAME "Floweret"
    DESCRIPTION "Botanica Granular & Wavetable Synthesizer"
)

# Source Files
target_sources(Floweret PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Common/Parameters.h
    Source/Common/AudioHelpers.h
    Source/DSP/FloweretSynth.cpp
    Source/DSP/FloweretSynth.h
    Source/DSP/FloweretVoice.cpp
    Source/DSP/FloweretVoice.h
    Source/DSP/Oscillators/WavetableOscillator.cpp
    Source/DSP/Oscillators/WavetableOscillator.h
    Source/DSP/Oscillators/GranularOscillator.cpp
    Source/DSP/Oscillators/GranularOscillator.h
    Source/DSP/Oscillators/SpectralOscillator.cpp
    Source/DSP/Oscillators/SpectralOscillator.h
    Source/DSP/Oscillators/SampleOscillator.cpp
    Source/DSP/Oscillators/SampleOscillator.h
    Source/DSP/Oscillators/MultisampleOscillator.cpp
    Source/DSP/Oscillators/MultisampleOscillator.h
    Source/DSP/Oscillators/NoiseOscillator.cpp
    Source/DSP/Oscillators/NoiseOscillator.h
    Source/DSP/Oscillators/SubOscillator.cpp
    Source/DSP/Oscillators/SubOscillator.h
    Source/DSP/FX/ColorResonator.cpp
    Source/DSP/FX/ColorResonator.h
    Source/DSP/FX/MultiModeFilter.cpp
    Source/DSP/FX/MultiModeFilter.h
    Source/DSP/FX/EffectsRack.cpp
    Source/DSP/FX/EffectsRack.h
    Source/DSP/Sequencer/Arpeggiator.h
    Source/DSP/Sequencer/ClipSequencer.h
    Source/DSP/Modulation/EnvelopeGenerator.h
    Source/DSP/Modulation/LFO.h
    Source/UI/BotanicaLookAndFeel.cpp
    Source/UI/BotanicaLookAndFeel.h
    Source/UI/Components/GrowthFieldVisualizer.cpp
    Source/UI/Components/GrowthFieldVisualizer.h
    Source/UI/Components/BotanicaKnob.h
    Source/UI/Components/WavetableDisplay.h
    Source/UI/Components/GrainCloudDisplay.h
    Source/UI/Components/SpectralVisualizer.h
    Source/UI/Components/ResonatorKeyboard.cpp
    Source/UI/Components/ResonatorKeyboard.h
)

target_compile_definitions(Floweret PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Floweret PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_events
    juce::juce_core
    juce::juce_data_structures
    juce::juce_audio_formats
    juce::juce_audio_devices
    juce::juce_audio_basics
)
