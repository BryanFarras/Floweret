#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../BotanicaLookAndFeel.h"
#include "../../DSP/FX/ColorResonator.h"

namespace Floweret
{
    class SpectralVisualizer : public juce::Component, public juce::Timer
    {
    public:
        SpectralVisualizer (BotanicaLookAndFeel& laf, const ColorResonator& resonator)
            : lookAndFeel (laf), colorResonator (resonator)
        {
            bins.resize (32, 0.0f);
            startTimerHz (30);
        }

        ~SpectralVisualizer() override
        {
            stopTimer();
        }

        void timerCallback() override
        {
            const float dry = colorResonator.getDryMagnitude();
            const float tuned = colorResonator.getTunedMagnitude();

            for (size_t i = 0; i < bins.size(); ++i)
            {
                // Dynamic decay
                bins[i] = std::max (0.0f, bins[i] * 0.88f);
                if (i % 2 == 0)
                    bins[i] = std::max (bins[i], tuned * (0.8f + 0.4f * std::sin (static_cast<float> (i))));
                else
                    bins[i] = std::max (bins[i], dry * (0.5f + 0.3f * std::cos (static_cast<float> (i))));
            }

            repaint();
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            g.setColour (juce::Colour::fromRGBA (20, 32, 26, 210));
            g.fillRoundedRectangle (bounds, 8.0f);

            // Draw frequency spectral bars (Chroma style)
            const float barWidth = bounds.getWidth() / static_cast<float> (bins.size());
            for (size_t i = 0; i < bins.size(); ++i)
            {
                const float barHeight = std::min (bounds.getHeight() * 0.85f, bins[i] * bounds.getHeight() * 1.5f);
                const float x = bounds.getX() + i * barWidth + 1.0f;
                const float y = bounds.getBottom() - barHeight - 4.0f;

                // Color Bass: tuned frequencies in soft cyan/sage, dry in violet/purple
                const juce::Colour barCol = (i % 2 == 0)
                    ? juce::Colour::fromRGB (110, 190, 160) // Tuned Cyan/Sage
                    : juce::Colour::fromRGB (160, 130, 200); // Dry Purple

                g.setColour (barCol.withAlpha (0.85f));
                g.fillRoundedRectangle (x, y, barWidth - 2.0f, barHeight, 2.0f);
            }

            // Legend
            g.setFont (juce::FontOptions (10.0f));
            g.setColour (juce::Colour::fromRGB (160, 130, 200));
            g.drawText ("DRY", bounds.getX() + 8.0f, bounds.getY() + 4.0f, 40, 14, juce::Justification::left, true);
            g.setColour (juce::Colour::fromRGB (110, 190, 160));
            g.drawText ("TUNED", bounds.getX() + 45.0f, bounds.getY() + 4.0f, 50, 14, juce::Justification::left, true);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        const ColorResonator& colorResonator;
        std::vector<float> bins;
    };
}
