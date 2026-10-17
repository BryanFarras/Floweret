#include "BotanicaLookAndFeel.h"

namespace Floweret
{
    BotanicaLookAndFeel::BotanicaLookAndFeel()
    {
        setColour (juce::ResizableWindow::backgroundColourId, getOatmealCanvas());
        setColour (juce::Label::textColourId, getDarkEspresso());
        setColour (juce::TextButton::textColourOffId, getDarkEspresso());
        setColour (juce::TextButton::textColourOnId, getDarkEspresso());
        setColour (juce::ComboBox::backgroundColourId, getWhisperWhite());
        setColour (juce::ComboBox::textColourId, getDarkEspresso());
        setColour (juce::ComboBox::outlineColourId, getPlatinumBorder());
        setColour (juce::PopupMenu::backgroundColourId, getWhisperWhite());
        setColour (juce::PopupMenu::textColourId, getDarkEspresso());
        setColour (juce::PopupMenu::highlightedBackgroundColourId, getPeachCoral());
        setColour (juce::PopupMenu::highlightedTextColourId, getDarkEspresso());
    }

    void BotanicaLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPosProportional, float rotaryStartAngle,
                                                float rotaryEndAngle, juce::Slider& slider)
    {
        juce::ignoreUnused (slider);
        const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (2.0f);
        const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto centre = bounds.getCentre();
        const float rx = centre.x - radius;
        const float ry = centre.y - radius;
        const float rw = radius * 2.0f;

        const float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // 1. Soft Ambient Shadow
        g.setColour (juce::Colour::fromRGBA (45, 37, 34, 15));
        g.fillEllipse (rx - 1.5f, ry + 1.5f, rw + 3.0f, rw + 3.0f);

        // 2. Track Background Arc (Subtle apricot / platinum groove)
        const float trackRadius = radius * 0.86f;
        juce::Path trackPath;
        trackPath.addCentredArc (centre.x, centre.y, trackRadius, trackRadius, 0.0f,
                                 rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (juce::Colour::fromRGBA (232, 232, 228, 200));
        g.strokePath (trackPath, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 3. Active Track Arc (Glowing Golden Peach #FEC89A / Coral #FEC5BB)
        if (currentAngle > rotaryStartAngle)
        {
            juce::Path activePath;
            activePath.addCentredArc (centre.x, centre.y, trackRadius, trackRadius, 0.0f,
                                      rotaryStartAngle, currentAngle, true);

            // Subtle glow
            g.setColour (getPeachCoral().withAlpha (0.45f));
            g.strokePath (activePath, juce::PathStrokeType (4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // Crisp line
            g.setColour (getGoldenPeach());
            g.strokePath (activePath, juce::PathStrokeType (2.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // 4. Dial Knob Body (Frosted peachy white / warm ivory)
        const float knobRadius = radius * 0.68f;
        const float kx = centre.x - knobRadius;
        const float ky = centre.y - knobRadius;
        const float kw = knobRadius * 2.0f;

        juce::ColourGradient knobGrad (juce::Colour::fromRGB (0xFF, 0xFD, 0xFB), centre.x, ky,
                                      getCreamyApricot(), centre.x, ky + kw, false);
        g.setGradientFill (knobGrad);
        g.fillEllipse (kx, ky, kw, kw);

        // Dial Rim
        g.setColour (getPlatinumBorder());
        g.drawEllipse (kx, ky, kw, kw, 1.2f);

        // 5. Needle Pointer
        const float needleLength = knobRadius * 0.72f;
        const float nx = centre.x + std::sin (currentAngle) * needleLength;
        const float ny = centre.y - std::cos (currentAngle) * needleLength;

        g.setColour (getDarkEspresso());
        g.drawLine (centre.x, centre.y, nx, ny, 2.0f);

        // Center Pivot Dot
        g.setColour (getDarkEspresso());
        g.fillEllipse (centre.x - 2.0f, centre.y - 2.0f, 4.0f, 4.0f);
    }

    void BotanicaLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                    const juce::Colour& backgroundColour,
                                                    bool shouldDrawButtonAsHighlighted,
                                                    bool shouldDrawButtonAsDown)
    {
        juce::ignoreUnused (backgroundColour);
        auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
        const float corner = 4.0f;

        const bool isToggle = button.getToggleState();

        juce::Colour bg = getWhisperWhite();
        if (isToggle || shouldDrawButtonAsDown)
            bg = getPeachCoral();
        else if (shouldDrawButtonAsHighlighted)
            bg = getPeachLight();
        else
            bg = getBlushPink().withAlpha (0.6f);

        g.setColour (bg);
        g.fillRoundedRectangle (bounds, corner);

        g.setColour (isToggle ? getGoldenPeach() : getPlatinumBorder());
        g.drawRoundedRectangle (bounds, corner, 1.0f);
    }

    void BotanicaLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
    {
        juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.setColour (getDarkEspresso());
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

    void BotanicaLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                            int buttonX, int buttonY, int buttonW, int buttonH,
                                            juce::ComboBox& box)
    {
        juce::ignoreUnused (isButtonDown);
        auto bounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height)).reduced (1.0f);
        const float corner = 4.0f;

        g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle (bounds, corner);

        g.setColour (box.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (bounds, corner, 1.0f);

        // Arrow
        const auto arrowBounds = juce::Rectangle<float> (static_cast<float> (buttonX), static_cast<float> (buttonY),
                                                         static_cast<float> (buttonW), static_cast<float> (buttonH));
        juce::Path arrow;
        arrow.addTriangle (arrowBounds.getCentreX() - 3.5f, arrowBounds.getCentreY() - 2.0f,
                           arrowBounds.getCentreX() + 3.5f, arrowBounds.getCentreY() - 2.0f,
                           arrowBounds.getCentreX(), arrowBounds.getCentreY() + 2.5f);
        g.setColour (getDarkEspresso());
        g.fillPath (arrow);
    }

    void BotanicaLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float minSliderPos, float maxSliderPos,
                                                juce::Slider::SliderStyle style, juce::Slider& slider)
    {
        juce::ignoreUnused (minSliderPos, maxSliderPos, slider);
        const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();

        if (style == juce::Slider::LinearVertical)
        {
            const float trackWidth = 4.0f;
            const float trackX = bounds.getCentreX() - trackWidth * 0.5f;

            // Draw graduated tick marks on left & right of fader
            g.setColour (juce::Colour::fromRGBA (107, 94, 88, 60));
            const int numTicks = 9;
            for (int i = 0; i < numTicks; ++i)
            {
                const float tickY = bounds.getY() + 8.0f + (bounds.getHeight() - 16.0f) * (static_cast<float>(i) / (numTicks - 1));
                g.drawHorizontalLine (static_cast<int> (tickY), bounds.getX() + 2.0f, trackX - 3.0f);
                g.drawHorizontalLine (static_cast<int> (tickY), trackX + trackWidth + 3.0f, bounds.getRight() - 2.0f);
            }

            // Recessed Background track groove
            g.setColour (juce::Colour::fromRGBA (45, 37, 34, 40));
            g.fillRoundedRectangle (trackX, bounds.getY() + 4.0f, trackWidth, bounds.getHeight() - 8.0f, 2.0f);

            // Active Track fill (up from bottom)
            const float trackBottom = bounds.getBottom() - 4.0f;
            if (trackBottom > sliderPos)
            {
                g.setColour (getPeachCoral().withAlpha (0.7f));
                g.fillRoundedRectangle (trackX, sliderPos, trackWidth, trackBottom - sliderPos, 2.0f);
            }

            // Silver/Frosted Peachy Fader Cap
            const float thumbW = juce::jmin (bounds.getWidth() - 8.0f, 22.0f);
            const float thumbH = 14.0f;
            auto thumbRect = juce::Rectangle<float> (bounds.getCentreX() - thumbW * 0.5f,
                                                     sliderPos - thumbH * 0.5f, thumbW, thumbH);

            // Thumb shadow
            g.setColour (juce::Colour::fromRGBA (45, 37, 34, 30));
            g.fillRoundedRectangle (thumbRect.translated (0.0f, 1.5f), 3.0f);

            // Thumb gradient
            juce::ColourGradient thumbGrad (juce::Colour::fromRGB (0xFF, 0xFD, 0xFA), thumbRect.getX(), thumbRect.getY(),
                                           getCreamyApricot(), thumbRect.getX(), thumbRect.getBottom(), false);
            g.setGradientFill (thumbGrad);
            g.fillRoundedRectangle (thumbRect, 3.0f);

            g.setColour (getPlatinumBorder());
            g.drawRoundedRectangle (thumbRect, 3.0f, 1.0f);

            // Center indicator stripe
            g.setColour (getDarkEspresso());
            g.drawHorizontalLine (static_cast<int> (sliderPos), thumbRect.getX() + 3.0f, thumbRect.getRight() - 3.0f);
        }
        else
        {
            // Horizontal Slider
            const float trackHeight = 4.0f;
            const float trackY = bounds.getCentreY() - trackHeight * 0.5f;

            g.setColour (juce::Colour::fromRGBA (45, 37, 34, 30));
            g.fillRoundedRectangle (bounds.getX(), trackY, bounds.getWidth(), trackHeight, 2.0f);

            g.setColour (getPeachCoral());
            g.fillRoundedRectangle (bounds.getX(), trackY, sliderPos - bounds.getX(), trackHeight, 2.0f);

            const float thumbRadius = 6.0f;
            g.setColour (getWhisperWhite());
            g.fillEllipse (sliderPos - thumbRadius, bounds.getCentreY() - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f);
            g.setColour (getDarkEspresso());
            g.drawEllipse (sliderPos - thumbRadius, bounds.getCentreY() - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f, 1.5f);
        }
    }
}

