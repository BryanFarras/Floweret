#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    class BotanicaKnob : public juce::Component
    {
    public:
        BotanicaKnob (const juce::String& titleText, BotanicaLookAndFeel& laf)
            : title (titleText), lookAndFeel (laf)
        {
            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            slider.setLookAndFeel (&lookAndFeel);
            addAndMakeVisible (slider);

            slider.onValueChange = [this]() {
                valueLabel.setText (juce::String (slider.getValue(), 2), juce::dontSendNotification);
            };

            valueLabel.setJustificationType (juce::Justification::centred);
            valueLabel.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            valueLabel.setColour (juce::Label::textColourId, lookAndFeel.getDeepMoss());
            addAndMakeVisible (valueLabel);
        }

        ~BotanicaKnob() override
        {
            slider.setLookAndFeel (nullptr);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            const int titleHeight = 18;
            const int valueHeight = 16;
            const int dialSize = bounds.getHeight() - titleHeight - valueHeight;

            slider.setBounds (bounds.getCentreX() - dialSize / 2, titleHeight, dialSize, dialSize);
            valueLabel.setBounds (0, bounds.getHeight() - valueHeight, bounds.getWidth(), valueHeight);
        }

        void paint (juce::Graphics& g) override
        {
            g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getDeepMoss());
            g.drawText (title.toUpperCase(), 0, 0, getWidth(), 16, juce::Justification::centred, true);
        }

        juce::Slider& getSlider() noexcept { return slider; }

    private:
        juce::String title;
        BotanicaLookAndFeel& lookAndFeel;
        juce::Slider slider;
        juce::Label valueLabel;
    };
}
