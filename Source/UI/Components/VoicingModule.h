#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    class VoicingModule : public juce::Component
    {
    public:
        VoicingModule (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            titleLabel.setText ("VOICING", juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            voicesBox.addItemList ({ "1 (Mono)", "2 Voices", "4 Voices", "8 Poly", "16 Poly" }, 1);
            voicesBox.setSelectedId (4, juce::dontSendNotification);
            addAndMakeVisible (voicesBox);

            setupDial (spreadSlider, spreadLabel, "SPREAD");
            setupDial (colorSlider, colorLabel, "COLOR");
            setupDial (detuneSlider, detuneLabel, "UNISON");
            setupDial (glideSlider, glideLabel, "GLIDE");

            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::spreadAmount.getParamID(), spreadSlider));
            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::colorAmount.getParamID(), colorSlider));
        }

        ~VoicingModule() override = default;

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
            titleLabel.setBounds (8, 4, 80, 18);
            voicesBox.setBounds (92, 3, getWidth() - 100, 20);

            const int dialW = (getWidth() - 16) / 4;
            const int dialSize = juce::jmin (dialW - 4, 46);
            const int dialY = 46;

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = 8 + idx * dialW;
                l.setBounds (dx, 28, dialW, 14);
                s.setBounds (dx + (dialW - dialSize) / 2, dialY, dialSize, dialSize);
            };

            placeDial (spreadSlider, spreadLabel, 0);
            placeDial (colorSlider, colorLabel, 1);
            placeDial (detuneSlider, detuneLabel, 2);
            placeDial (glideSlider, glideLabel, 3);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::Label titleLabel;
        juce::ComboBox voicesBox;
        juce::Slider spreadSlider, colorSlider, detuneSlider, glideSlider;
        juce::Label spreadLabel, colorLabel, detuneLabel, glideLabel;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

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
