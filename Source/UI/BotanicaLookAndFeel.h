#pragma once

#include <JuceHeader.h>

namespace Floweret
{
    class BotanicaLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        BotanicaLookAndFeel();
        ~BotanicaLookAndFeel() override = default;

        // Custom Rotary Slider (Dials like COLOR, MUTATE, BLOOM in VINE)
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider& slider) override;

        // Custom Organic Pill Buttons (ADD, CLEAR, RESET, SPROUT, LIGHT)
        void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                   const juce::Colour& backgroundColour,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown) override;

        void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;

        // Custom ComboBox (Scale & Key Dropdown)
        void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                           int buttonX, int buttonY, int buttonW, int buttonH,
                           juce::ComboBox& box) override;

        // Custom Linear Slider (Volume, Faders)
        void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPos, float minSliderPos, float maxSliderPos,
                               juce::Slider::SliderStyle style, juce::Slider& slider) override;

        // Color Palette accessors
        juce::Colour getIvoryBg() const noexcept         { return juce::Colour::fromRGB (243, 242, 236); }
        juce::Colour getDeepMoss() const noexcept        { return juce::Colour::fromRGB (27, 46, 36); }
        juce::Colour getSageAccent() const noexcept      { return juce::Colour::fromRGB (118, 154, 130); }
        juce::Colour getSoftCardBg() const noexcept      { return juce::Colour::fromRGBA (255, 255, 255, 170); }
        juce::Colour getCardBorder() const noexcept      { return juce::Colour::fromRGB (214, 218, 210); }
        juce::Colour getFieldGradientTop() const noexcept{ return juce::Colour::fromRGB (226, 232, 226); }
        juce::Colour getFieldGradientBot() const noexcept{ return juce::Colour::fromRGB (148, 168, 150); }
    };
}
