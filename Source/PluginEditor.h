#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/BotanicaLookAndFeel.h"
#include "UI/Components/OscillatorModule.h"
#include "UI/Components/NoiseSubModule.h"
#include "UI/Components/PreFilterModule.h"
#include "UI/Components/VoicingModule.h"
#include "UI/Components/EnvelopeModule.h"
#include "UI/Components/LfoModule.h"
#include "UI/Components/EmptyModuleBox.h"
#include "UI/Components/GuitarRigRack.h"
#include "UI/Components/SamplerDisplay.h"
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
            Oscillator = 0,
            Effects,
            Routes,
            Sampler
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

        ActiveView currentView = ActiveView::Oscillator;
        bool isDraggingFile = false;

        // Top Switchable View Tabs from design1.png
        juce::TextButton oscTabBtn;
        juce::TextButton effectsTabBtn;
        juce::TextButton routesTabBtn;
        juce::TextButton samplerTabBtn;

        // Top Right Master Controls
        juce::Label masterLabel;
        juce::Slider masterGainSlider;
        juce::Label masterReadoutLabel;
        juce::Label voiceCounterLabel;
        juce::TextButton sproutButton;

        // OSCILATOR Tab Modules (Exact layout from design1.png)
        // Left Column
        OscillatorModule oscA;
        OscillatorModule oscB;
        OscillatorModule oscC;
        NoiseSubModule noiseSub;
        PreFilterModule preFilter;
        EmptyModuleBox emptyBoxLeft;

        // Right Column
        VoicingModule voicing;
        EnvelopeModule envModule;
        LfoModule lfoModule;
        EmptyModuleBox emptyBoxRight;

        // Alternative Views
        GuitarRigRack guitarRigRack;
        SamplerDisplay samplerDisplay;
        EmptyModuleBox routesDisplay;

        // Bottom Strip: Full-Width Virtual Piano Keyboard from design1.png
        ResonatorKeyboard keyboard;

        // APVTS Attachments
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;

        void setupAttachments();
        void switchView (ActiveView newView);
        void performGenerativeSprout();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloweretAudioProcessorEditor)
    };
}
