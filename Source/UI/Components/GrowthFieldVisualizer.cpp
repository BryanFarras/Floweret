#include "GrowthFieldVisualizer.h"

namespace Floweret
{
    GrowthFieldVisualizer::GrowthFieldVisualizer (BotanicaLookAndFeel& laf)
        : lookAndFeel (laf)
    {
        setInterceptsMouseClicks (false, false);

        // Populate floating frosted rectangles matching VINE aesthetic
        cards = {
            { 0.12f, 0.25f, 0.16f, 0.38f, 0.18f, 0.003f, 0.0f },
            { 0.30f, 0.20f, 0.14f, 0.45f, 0.22f, 0.002f, 1.2f },
            { 0.42f, 0.15f, 0.18f, 0.52f, 0.16f, 0.0025f, 2.4f },
            { 0.58f, 0.22f, 0.15f, 0.44f, 0.20f, 0.0035f, 3.8f },
            { 0.72f, 0.18f, 0.16f, 0.48f, 0.19f, 0.0028f, 5.1f }
        };

        startTimerHz (30);
    }

    GrowthFieldVisualizer::~GrowthFieldVisualizer()
    {
        stopTimer();
    }

    void GrowthFieldVisualizer::timerCallback()
    {
        animationPhase += 0.02f;
        if (animationPhase >= juce::MathConstants<float>::twoPi)
            animationPhase -= juce::MathConstants<float>::twoPi;

        for (auto& c : cards)
        {
            c.phase += c.speed;
            if (c.phase >= juce::MathConstants<float>::twoPi)
                c.phase -= juce::MathConstants<float>::twoPi;
        }

        repaint();
    }

    void GrowthFieldVisualizer::paint (juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float w = bounds.getWidth();
        const float h = bounds.getHeight();
        const float corner = 18.0f;

        // 1. Base Gradient Canvas (Warm cream at top to soft muted sage green at bottom)
        juce::ColourGradient bgGrad (juce::Colour::fromRGB (235, 237, 231), 0.0f, 0.0f,
                                    juce::Colour::fromRGB (168, 185, 172), 0.0f, h, false);
        g.setGradientFill (bgGrad);
        g.fillRoundedRectangle (bounds, corner);

        // 2. Animated Floating Translucent Rectangles (VINE aesthetic)
        for (const auto& c : cards)
        {
            const float floatY = c.yNorm + std::sin (c.phase + animationPhase) * 0.03f * (1.0f + currentActivity * 2.0f);
            const auto cardRect = juce::Rectangle<float> (c.xNorm * w, floatY * h, c.widthNorm * w, c.heightNorm * h);

            g.setColour (juce::Colour::fromRGBA (255, 255, 255, static_cast<juce::uint8> (c.opacity * 255)));
            g.fillRect (cardRect);

            g.setColour (juce::Colour::fromRGBA (255, 255, 255, static_cast<juce::uint8> (c.opacity * 120)));
            g.drawRect (cardRect, 1.0f);
        }

        // 3. Gentle Organic Hill / Sine Wave Landscape Layer
        juce::Path hillPath;
        hillPath.startNewSubPath (0.0f, h * 0.68f);

        for (float x = 0.0f; x <= w; x += 15.0f)
        {
            const float normX = x / w;
            const float wave1 = std::sin (normX * juce::MathConstants<float>::twoPi + animationPhase * 0.5f) * (h * 0.035f);
            const float wave2 = std::cos (normX * juce::MathConstants<float>::pi * 3.0f + animationPhase * 0.3f) * (h * 0.015f);
            const float y = (h * 0.68f) + wave1 + wave2;
            hillPath.lineTo (x, y);
        }

        hillPath.lineTo (w, h);
        hillPath.lineTo (0.0f, h);
        hillPath.closeSubPath();

        // Hill fill
        juce::ColourGradient hillGrad (juce::Colour::fromRGBA (135, 155, 138, 200), 0.0f, h * 0.65f,
                                      juce::Colour::fromRGBA (115, 136, 118, 240), 0.0f, h, false);
        g.setGradientFill (hillGrad);
        g.fillPath (hillPath);

        // 4. Subtle Outer Border
        g.setColour (juce::Colour::fromRGBA (200, 206, 198, 160));
        g.drawRoundedRectangle (bounds, corner, 1.5f);

        // 5. Breadcrumb Tag: "VINE / GROWTH FIELD"
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.setColour (juce::Colour::fromRGBA (27, 46, 36, 110));
        g.drawText ("FLOWERET / GROWTH FIELD", 18, 14, 250, 16, juce::Justification::left, true);
    }
}
