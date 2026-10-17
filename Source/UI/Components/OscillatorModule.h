#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"
#include "WavetableDisplay.h"

namespace Floweret
{
    class OscillatorModule : public juce::Component
    {
    public:
        enum class OscMode { Wavetable = 0, Spectral };

        OscillatorModule (const juce::String& oscName, BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : name (oscName), lookAndFeel (laf), wtPreview (laf)
        {
            // Title
            titleLabel.setText (name, juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            // Power Toggle
            powerBtn.setButtonText ("I");
            powerBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (powerBtn);

            // Mode Toggle
            wtModeBtn.setButtonText ("WT");
            specModeBtn.setButtonText ("SPEC");
            wtModeBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (wtModeBtn);
            addAndMakeVisible (specModeBtn);

            wtModeBtn.onClick = [this]() { setMode (OscMode::Wavetable); };
            specModeBtn.onClick = [this]() { setMode (OscMode::Spectral); };

            // Warp Mode selector
            warpModeBox.addItemList (getWarpModeNames(), 1);
            warpModeBox.setSelectedId (1, juce::dontSendNotification);
            addAndMakeVisible (warpModeBox);

            // Wavetable preview display
            addAndMakeVisible (wtPreview);

            // Dials
            setupDial (posSlider, posLabel, "POS");
            setupDial (warpSlider, warpLabel, "WARP");
            setupDial (detuneSlider, detuneLabel, "TUNE");
            setupDial (panSlider, panLabel, "PAN");
            setupDial (levelSlider, levelLabel, "LEVEL");

            posSlider.onValueChange = [this]() {
                wtPreview.updateWaveform (static_cast<float> (posSlider.getValue()));
            };

            wtPreview.onPositionChanged = [this](float p) {
                posSlider.setValue (p, juce::sendNotificationSync);
            };

            // APVTS Attachments
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::wtPosition.getParamID(), posSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::wtWarpAmount.getParamID(), warpSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::wtDetune.getParamID(), detuneSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::wtPan.getParamID(), panSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::wtLevel.getParamID(), levelSlider));
            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, ParamIDs::wtWarpMode.getParamID(), warpModeBox));
            buttonAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                apvts, ParamIDs::wtActive.getParamID(), powerBtn));
        }

        ~OscillatorModule() override = default;

        void setMode (OscMode newMode)
        {
            currentMode = newMode;
            wtModeBtn.setToggleState (currentMode == OscMode::Wavetable, juce::dontSendNotification);
            specModeBtn.setToggleState (currentMode == OscMode::Spectral, juce::dontSendNotification);

            if (currentMode == OscMode::Wavetable)
            {
                posLabel.setText ("POS", juce::dontSendNotification);
                warpLabel.setText ("WARP", juce::dontSendNotification);
            }
            else
            {
                posLabel.setText ("TILT", juce::dontSendNotification);
                warpLabel.setText ("SMEAR", juce::dontSendNotification);
            }
            repaint();
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // Card background (#F8EDEB)
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (bounds, 5.0f);

            // Border
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            // Header underline
            g.drawHorizontalLine (24, bounds.getX(), bounds.getRight());
        }

        void resized() override
        {
            const auto bounds = getLocalBounds().reduced (3);

            // Header line (y: 2 to 22)
            powerBtn.setBounds (6, 4, 18, 18);
            titleLabel.setBounds (28, 3, 50, 18);

            wtModeBtn.setBounds (82, 4, 28, 18);
            specModeBtn.setBounds (112, 4, 40, 18);

            warpModeBox.setBounds (156, 4, 110, 18);

            // Body (y: 26 to bottom)
            const int bodyY = 26;
            const int bodyH = bounds.getHeight() - bodyY;

            // Left: Wavetable / Spectral Visualizer (width: ~190px)
            const int previewW = 180;
            wtPreview.setBounds (6, bodyY + 2, previewW, bodyH - 4);

            // Right: 5 Dials (POS, WARP, TUNE, PAN, LEVEL)
            const int dialStartX = 6 + previewW + 8;
            const int totalDialW = bounds.getWidth() - dialStartX - 4;
            const int dialW = totalDialW / 5;
            const int dialSize = juce::jmin (dialW - 4, bodyH - 18);

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = dialStartX + idx * dialW;
                l.setBounds (dx, bodyY + 2, dialW, 14);
                s.setBounds (dx + (dialW - dialSize) / 2, bodyY + 16, dialSize, dialSize);
            };

            placeDial (posSlider, posLabel, 0);
            placeDial (warpSlider, warpLabel, 1);
            placeDial (detuneSlider, detuneLabel, 2);
            placeDial (panSlider, panLabel, 3);
            placeDial (levelSlider, levelLabel, 4);
        }

    private:
        juce::String name;
        BotanicaLookAndFeel& lookAndFeel;
        OscMode currentMode = OscMode::Wavetable;

        juce::Label titleLabel;
        juce::TextButton powerBtn;
        juce::TextButton wtModeBtn;
        juce::TextButton specModeBtn;
        juce::ComboBox warpModeBox;

        WavetableDisplay wtPreview;

        juce::Slider posSlider, warpSlider, detuneSlider, panSlider, levelSlider;
        juce::Label posLabel, warpLabel, detuneLabel, panLabel, levelLabel;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;

        void setupDial (juce::Slider& s, juce::Label& l, const juce::String& text)
        {
            l.setText (text, juce::dontSendNotification);
            l.setFont (juce::FontOptions (9.0f, juce::Font::bold));
            l.setColour (juce::Label::textColourId, lookAndFeel.getMutedSlate());
            l.setJustificationType (juce::Justification::centred);
            addAndMakeVisible (l);

            s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            s.setLookAndFeel (&lookAndFeel);
            addAndMakeVisible (s);
        }
    };
}
