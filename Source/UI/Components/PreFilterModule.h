#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    class PreFilterModule : public juce::Component
    {
    public:
        PreFilterModule (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            titleLabel.setText ("PRE FILTER (3)", juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            typeBox.addItemList (getFilterTypeNames(), 1);
            typeBox.setSelectedId (2, juce::dontSendNotification); // LP 24dB
            addAndMakeVisible (typeBox);

            setupDial (cutoffSlider, cutoffLabel, "CUTOFF");
            setupDial (resSlider, resLabel, "RESONANCE");
            setupDial (driveSlider, driveLabel, "DRIVE");

            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, ParamIDs::filterType.getParamID(), typeBox));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::filterCutoff.getParamID(), cutoffSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::filterResonance.getParamID(), resSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::filterDrive.getParamID(), driveSlider));
        }

        ~PreFilterModule() override = default;

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // Card background (#F8EDEB)
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (bounds, 5.0f);

            // Border
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            // Header line
            g.drawHorizontalLine (24, bounds.getX(), bounds.getRight());
        }

        void resized() override
        {
            titleLabel.setBounds (8, 4, 110, 18);
            typeBox.setBounds (120, 3, getWidth() - 128, 20);

            const int dialW = (getWidth() - 16) / 3;
            const int dialSize = juce::jmin (dialW - 4, 48);
            const int dialY = 46;

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = 8 + idx * dialW;
                l.setBounds (dx, 28, dialW, 14);
                s.setBounds (dx + (dialW - dialSize) / 2, dialY, dialSize, dialSize);
            };

            placeDial (cutoffSlider, cutoffLabel, 0);
            placeDial (resSlider, resLabel, 1);
            placeDial (driveSlider, driveLabel, 2);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::Label titleLabel;
        juce::ComboBox typeBox;
        juce::Slider cutoffSlider, resSlider, driveSlider;
        juce::Label cutoffLabel, resLabel, driveLabel;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;

        void setupDial (juce::Slider& s, juce::Label& l, const juce::String& text)
        {
            l.setText (text, juce::dontSendNotification);
            l.setFont (juce::FontOptions (8.5f, juce::Font::bold));
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
