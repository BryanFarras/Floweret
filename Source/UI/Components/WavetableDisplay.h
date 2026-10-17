#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    class WavetableDisplay : public juce::Component
    {
    public:
        WavetableDisplay (BotanicaLookAndFeel& laf)
            : lookAndFeel (laf)
        {
            points.resize (128, 0.0f);
            updateWaveform (0.0f);
        }

        void updateWaveform (float position)
        {
            // Compute morphed shape preview (Sine -> Tri -> Saw -> Formant)
            for (size_t i = 0; i < points.size(); ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (points.size());
                const float angle = t * juce::MathConstants<float>::twoPi;
                const float sinV = std::sin (angle);
                const float sawV = (1.0f - 2.0f * t) * 0.8f;
                points[i] = sinV * (1.0f - position) + sawV * position;
            }
            repaint();
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            g.setColour (juce::Colour::fromRGBA (20, 32, 26, 220));
            g.fillRoundedRectangle (bounds, 8.0f);

            // Draw grid lines
            g.setColour (juce::Colour::fromRGBA (120, 150, 130, 40));
            g.drawHorizontalLine (static_cast<int> (bounds.getCentreY()), bounds.getX(), bounds.getRight());

            // Draw wave path
            juce::Path wavePath;
            for (size_t i = 0; i < points.size(); ++i)
            {
                const float x = bounds.getX() + (static_cast<float> (i) / static_cast<float> (points.size() - 1)) * bounds.getWidth();
                const float y = bounds.getCentreY() - points[i] * (bounds.getHeight() * 0.38f);
                if (i == 0)
                    wavePath.startNewSubPath (x, y);
                else
                    wavePath.lineTo (x, y);
            }

            // Glow stroke
            g.setColour (juce::Colour::fromRGBA (126, 206, 168, 70));
            g.strokePath (wavePath, juce::PathStrokeType (4.0f));

            // Sharp line
            g.setColour (juce::Colour::fromRGB (160, 235, 195));
            g.strokePath (wavePath, juce::PathStrokeType (1.8f));

            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (juce::Colour::fromRGBA (255, 255, 255, 120));
            g.drawText ("WAVETABLE 2048", bounds.getX() + 8.0f, bounds.getY() + 4.0f, 120, 14, juce::Justification::left, true);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        std::vector<float> points;
    };
}
