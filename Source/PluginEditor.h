#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/BotanicaLookAndFeel.h"
#include "UI/Components/GrowthFieldVisualizer.h"
#include "UI/Components/BotanicaKnob.h"
#include "UI/Components/WavetableDisplay.h"
#include "UI/Components/GrainCloudDisplay.h"
#include "UI/Components/SpectralVisualizer.h"
#include "UI/Components/ResonatorKeyboard.h"

namespace Floweret
{
    class FloweretAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         public juce::Timer,
                                         public juce::FileDragAndDropTarget
    {
    public:
        enum class ActiveView
        {
            GrowthField = 0,
            Wavetable,
            Granular,
            Spectral,
            Effects,
            Sequencer
        };

        FloweretAudioProcessorEditor (FloweretAudioProcessor&);
        ~FloweretAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;
        void timerCallback() override;

        // FileDragAndDropTarget overrides
        bool isInterestedInFileDrag (const juce::StringArray& files) override;
        void fileDragEnter (const juce::StringArray& files, int x, int y) override;
        void fileDragMove (const juce::StringArray& files, int x, int y) override;
        void fileDragExit (const juce::StringArray& files) override;
        void filesDropped (const juce::StringArray& files, int x, int y) override;

    private:
        FloweretAudioProcessor& audioProcessor;
        BotanicaLookAndFeel lookAndFeel;

        ActiveView currentView = ActiveView::GrowthField;

        bool isDraggingFile = false;
        bool isOverLoopZone = false;
        bool isOverOneShotZone = false;
        std::unique_ptr<juce::FileChooser> fileChooser;

        // Header components
        juce::Label titleLabel;
        juce::Label guideBanner;
        juce::Slider outputVolumeSlider;
        juce::Label outputVolumeLabel;
        juce::Label voiceCounterLabel;
        juce::TextButton themeButton;

        // Generative strip components
        juce::Label subtitleLabel;
        juce::TextButton addButton;
        juce::TextButton clearButton;
        juce::TextButton resetButton;
        juce::TextButton sproutButton;
        juce::ComboBox keySelector;
        juce::ComboBox scaleSelector;

        // View Selector Pill Buttons
        juce::TextButton viewGrowthBtn;
        juce::TextButton viewWtBtn;
        juce::TextButton viewGranBtn;
        juce::TextButton viewSpecBtn;
        juce::TextButton viewFxBtn;
        juce::TextButton viewSeqBtn;

        // Drop zones
        juce::Label oneShotDropLabel;
        juce::Label loopDropLabel;

        // Growth Field Center Visualizer & Knobs
        GrowthFieldVisualizer growthField;
        BotanicaKnob colorKnob;
        BotanicaKnob mutateKnob;
        BotanicaKnob bloomKnob;
        BotanicaKnob spreadKnob;
        BotanicaKnob tailKnob;

        // Engine Displays
        WavetableDisplay wavetableDisplay;
        GrainCloudDisplay grainCloudDisplay;
        SpectralVisualizer spectralVisualizer;

        // Secondary Oscillator & Parameter Knobs
        BotanicaKnob wtPosKnob;
        BotanicaKnob wtWarpKnob;
        BotanicaKnob granSizeKnob;
        BotanicaKnob granDensityKnob;
        BotanicaKnob granSprayKnob;
        BotanicaKnob specTiltKnob;
        BotanicaKnob specSmearKnob;
        BotanicaKnob filterCutoffKnob;
        BotanicaKnob filterResKnob;
        BotanicaKnob reverbMixKnob;
        BotanicaKnob delayMixKnob;

        // Bottom Keyboard
        ResonatorKeyboard keyboard;

        // APVTS Attachments
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;

        void setupAttachments();
        void switchView (ActiveView newView);
        void performGenerativeSprout();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloweretAudioProcessorEditor)
    };
}
