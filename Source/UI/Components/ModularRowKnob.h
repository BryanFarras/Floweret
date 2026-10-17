#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    class ModularRowKnob : public juce::Component
    {
    public:
        ModularRowKnob (const juce::String& title, BotanicaLookAndFeel& laf, const juce::String& defaultReadout = "")
            : lookAndFeel (laf)
        {
            titleLabel.setText (title, juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            titleLabel.setJustificationType (juce::Justification::centredLeft);
            addAndMakeVisible (titleLabel);

            valueBox.setText (defaultReadout.isNotEmpty() ? defaultReadout : "0.0", juce::dontSendNotification);
            valueBox.setFont (juce::FontOptions (10.5f));
            valueBox.setColour (juce::Label::textColourId, lookAndFeel.getGoldenPeach());
            valueBox.setColour (juce::Label::backgroundColourId, juce::Colour::fromRGB (0x23, 0x27, 0x2A)); // Dark recessed box
            valueBox.setColour (juce::Label::outlineColourId, lookAndFeel.getPlatinumBorder().withAlpha (0.4f));
            valueBox.setJustificationType (juce::Justification::centredLeft);
            addAndMakeVisible (valueBox);

            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            slider.setLookAndFeel (&lookAndFeel);
            addAndMakeVisible (slider);

            slider.onValueChange = [this]() {
                if (formatter)
                    valueBox.setText (" " + formatter (slider.getValue()), juce::dontSendNotification);
                else
                    valueBox.setText (" " + juce::String (slider.getValue(), 1), juce::dontSendNotification);
            };
        }

        ~ModularRowKnob() override
        {
            slider.setLookAndFeel (nullptr);
        }

        void setFormatter (std::function<juce::String(double)> newFormatter)
        {
            formatter = std::move (newFormatter);
            if (formatter)
                valueBox.setText (" " + formatter (slider.getValue()), juce::dontSendNotification);
        }

        void setReadoutText (const juce::String& text)
        {
            valueBox.setText (" " + text, juce::dontSendNotification);
        }

        juce::Slider& getSlider() noexcept { return slider; }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (2, 1);
            const int dialSize = juce::jmin (bounds.getHeight() - 2, 38);
            const int leftWidth = bounds.getWidth() - dialSize - 6;

            const int titleH = 13;
            const int boxH = 17;

            titleLabel.setBounds (bounds.getX(), bounds.getY() + 1, leftWidth, titleH);
            valueBox.setBounds (bounds.getX(), bounds.getY() + titleH + 1, leftWidth, boxH);

            slider.setBounds (bounds.getX() + leftWidth + 4, bounds.getCentreY() - dialSize / 2, dialSize, dialSize);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::Label titleLabel;
        juce::Label valueBox;
        juce::Slider slider;
        std::function<juce::String(double)> formatter;
    };
}
