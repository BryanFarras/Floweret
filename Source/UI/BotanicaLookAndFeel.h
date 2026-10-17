#pragma once

#include <JuceHeader.h>

namespace Floweret
{
    class BotanicaLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        BotanicaLookAndFeel();
        ~BotanicaLookAndFeel() override = default;

        // Custom Rotary Slider
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider& slider) override;

        // Custom Organic Pill Buttons and Tabs
        void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                   const juce::Colour& backgroundColour,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown) override;

        void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;

        // Custom ComboBox
        void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                           int buttonX, int buttonY, int buttonW, int buttonH,
                           juce::ComboBox& box) override;

        // Custom Linear Slider (ADSR Vertical Faders)
        void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPos, float minSliderPos, float maxSliderPos,
                               juce::Slider::SliderStyle style, juce::Slider& slider) override;

        // "Soft Peachy Delight" Palette Accessors
        juce::Colour getPeachCoral() const noexcept    { return juce::Colour::fromRGB (0xFE, 0xC5, 0xBB); } // #FEC5BB
        juce::Colour getPeachLight() const noexcept    { return juce::Colour::fromRGB (0xFC, 0xD5, 0xCE); } // #FCD5CE
        juce::Colour getBlushPink() const noexcept     { return juce::Colour::fromRGB (0xFA, 0xE1, 0xDD); } // #FAE1DD
        juce::Colour getWhisperWhite() const noexcept  { return juce::Colour::fromRGB (0xF8, 0xED, 0xEB); } // #F8EDEB
        juce::Colour getPlatinumBorder() const noexcept{ return juce::Colour::fromRGB (0xE8, 0xE8, 0xE4); } // #E8E8E4
        juce::Colour getSageMint() const noexcept      { return juce::Colour::fromRGB (0xD8, 0xE2, 0xDC); } // #D8E2DC
        juce::Colour getOatmealCanvas() const noexcept { return juce::Colour::fromRGB (0xEC, 0xE4, 0xDB); } // #ECE4DB
        juce::Colour getCreamyApricot() const noexcept { return juce::Colour::fromRGB (0xFF, 0xE5, 0xD9); } // #FFE5D9
        juce::Colour getPastelApricot() const noexcept { return juce::Colour::fromRGB (0xFF, 0xD7, 0xBA); } // #FFD7BA
        juce::Colour getGoldenPeach() const noexcept   { return juce::Colour::fromRGB (0xFE, 0xC8, 0x9A); } // #FEC89A
        juce::Colour getDarkEspresso() const noexcept  { return juce::Colour::fromRGB (0x2D, 0x25, 0x22); } // #2D2522
        juce::Colour getMutedSlate() const noexcept    { return juce::Colour::fromRGB (0x6B, 0x5E, 0x58); } // #6B5E58
        juce::Colour getDarkDisplayBg() const noexcept { return juce::Colour::fromRGB (0x23, 0x27, 0x2A); } // Deep slate for visualizer display

        // Backward compatibility
        juce::Colour getIvoryBg() const noexcept          { return getOatmealCanvas(); }
        juce::Colour getDeepMoss() const noexcept         { return getDarkEspresso(); }
        juce::Colour getSageAccent() const noexcept       { return getGoldenPeach(); }
        juce::Colour getSoftCardBg() const noexcept       { return getWhisperWhite(); }
        juce::Colour getCardBorder() const noexcept       { return getPlatinumBorder(); }
        juce::Colour getFieldGradientTop() const noexcept { return getWhisperWhite(); }
        juce::Colour getFieldGradientBot() const noexcept { return getPastelApricot(); }
    };
}

