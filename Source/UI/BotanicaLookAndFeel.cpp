#include "BotanicaLookAndFeel.h"

namespace Floweret
{
    BotanicaLookAndFeel::BotanicaLookAndFeel()
    {
        setColour (juce::ResizableWindow::backgroundColourId, getIvoryBg());
        setColour (juce::Label::textColourId, getDeepMoss());
        setColour (juce::TextButton::textColourOffId, getDeepMoss());
        setColour (juce::ComboBox::backgroundColourId, juce::Colour::fromRGB (248, 247, 244));
        setColour (juce::ComboBox::textColourId, getDeepMoss());
        setColour (juce::ComboBox::outlineColourId, getCardBorder());
    }

    void BotanicaLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPosProportional, float rotaryStartAngle,
                                                float rotaryEndAngle, juce::Slider& slider)
    {
        const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (4.0f);
        const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto centre = bounds.getCentre();
        const float rx = centre.x - radius;
        const float ry = centre.y - radius;
        const float rw = radius * 2.0f;

        const float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // 1. Soft Circular Ambient Glow / Drop Shadow
        g.setColour (juce::Colour::fromRGBA (27, 46, 36, 18));
        g.fillEllipse (rx - 2.0f, ry + 2.0f, rw + 4.0f, rw + 4.0f);

        // 2. Dial Body: Frosted Warm Ivory / Soft Glass
        juce::ColourGradient dialGrad (juce::Colour::fromRGB (255, 255, 253), centre.x, ry,
                                      juce::Colour::fromRGB (236, 238, 233), centre.x, ry + rw, false);
        g.setGradientFill (dialGrad);
        g.fillEllipse (rx, ry, rw, rw);

        // 3. Dial Outer Delicate Rim
        g.setColour (juce::Colour::fromRGBA (200, 206, 196, 180));
        g.drawEllipse (rx, ry, rw, rw, 1.2f);

        // 4. Subtle Value Arc Track
        juce::Path trackPath;
        const float trackRadius = radius * 0.88f;
        trackPath.addCentredArc (centre.x, centre.y, trackRadius, trackRadius, 0.0f,
                                 rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (juce::Colour::fromRGBA (180, 190, 180, 70));
        g.strokePath (trackPath, juce::PathStrokeType (2.0f));

        // 5. Active Value Arc (Soft Sage Green)
        juce::Path activePath;
        activePath.addCentredArc (centre.x, centre.y, trackRadius, trackRadius, 0.0f,
                                  rotaryStartAngle, currentAngle, true);
        g.setColour (getSageAccent());
        g.strokePath (activePath, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 6. Needle Pointer (Pine green sleek line from center to edge)
        const float needleLength = radius * 0.65f;
        const float nx = centre.x + std::sin (currentAngle) * needleLength;
        const float ny = centre.y - std::cos (currentAngle) * needleLength;

        g.setColour (getDeepMoss());
        g.drawLine (centre.x, centre.y, nx, ny, 1.8f);

        // Center Pivot Dot
        g.setColour (getDeepMoss());
        g.fillEllipse (centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
    }

    void BotanicaLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                    const juce::Colour& backgroundColour,
                                                    bool shouldDrawButtonAsHighlighted,
                                                    bool shouldDrawButtonAsDown)
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
        const float corner = bounds.getHeight() * 0.5f; // Pill shape

        juce::Colour bg = juce::Colour::fromRGB (248, 247, 244);
        if (shouldDrawButtonAsDown)
            bg = juce::Colour::fromRGB (230, 235, 230);
        else if (shouldDrawButtonAsHighlighted)
            bg = juce::Colour::fromRGB (255, 255, 255);

        g.setColour (bg);
        g.fillRoundedRectangle (bounds, corner);

        g.setColour (getCardBorder());
        g.drawRoundedRectangle (bounds, corner, 1.2f);
    }

    void BotanicaLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
    {
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.setColour (getDeepMoss());
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

    void BotanicaLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                            int buttonX, int buttonY, int buttonW, int buttonH,
                                            juce::ComboBox& box)
    {
        auto bounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height)).reduced (1.0f);
        const float corner = bounds.getHeight() * 0.5f;

        g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle (bounds, corner);

        g.setColour (box.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (bounds, corner, 1.2f);

        // Arrow
        const auto arrowBounds = juce::Rectangle<float> (static_cast<float> (buttonX), static_cast<float> (buttonY),
                                                         static_cast<float> (buttonW), static_cast<float> (buttonH));
        juce::Path arrow;
        arrow.addTriangle (arrowBounds.getCentreX() - 4.0f, arrowBounds.getCentreY() - 2.0f,
                           arrowBounds.getCentreX() + 4.0f, arrowBounds.getCentreY() - 2.0f,
                           arrowBounds.getCentreX(), arrowBounds.getCentreY() + 3.0f);
        g.setColour (getDeepMoss());
        g.fillPath (arrow);
    }

    void BotanicaLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float minSliderPos, float maxSliderPos,
                                                juce::Slider::SliderStyle style, juce::Slider& slider)
    {
        const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
        const float trackHeight = 4.0f;
        const float trackY = bounds.getCentreY() - trackHeight * 0.5f;

        // Background track
        g.setColour (juce::Colour::fromRGBA (200, 208, 198, 140));
        g.fillRoundedRectangle (bounds.getX(), trackY, bounds.getWidth(), trackHeight, 2.0f);

        // Active track
        g.setColour (getDeepMoss());
        g.fillRoundedRectangle (bounds.getX(), trackY, sliderPos - bounds.getX(), trackHeight, 2.0f);

        // Thumb
        const float thumbRadius = 7.0f;
        g.setColour (getIvoryBg());
        g.fillEllipse (sliderPos - thumbRadius, bounds.getCentreY() - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f);
        g.setColour (getDeepMoss());
        g.drawEllipse (sliderPos - thumbRadius, bounds.getCentreY() - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f, 2.0f);
    }
}
