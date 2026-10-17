#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    struct DisplayGrain
    {
        float x = 0.5f;
        float y = 0.5f;
        float size = 4.0f;
        float alpha = 0.8f;
    };

    class GrainCloudDisplay : public juce::Component, public juce::Timer
    {
    public:
        GrainCloudDisplay (BotanicaLookAndFeel& laf)
            : lookAndFeel (laf)
        {
            grains.resize (32);
            for (auto& g : grains)
            {
                g.x = rng.nextUnsignedFloat();
                g.y = rng.nextUnsignedFloat();
                g.size = 3.0f + rng.nextUnsignedFloat() * 5.0f;
                g.alpha = 0.3f + rng.nextUnsignedFloat() * 0.7f;
            }
            startTimerHz (25);
        }

        ~GrainCloudDisplay() override
        {
            stopTimer();
        }

        void timerCallback() override
        {
            for (auto& g : grains)
            {
                g.alpha -= 0.04f;
                if (g.alpha <= 0.0f)
                {
                    g.x = currentCenterPos + (rng.nextFloat() * currentSpray * 0.4f);
                    g.x = std::max (0.05f, std::min (g.x, 0.95f));
                    g.y = 0.2f + rng.nextUnsignedFloat() * 0.6f;
                    g.alpha = 0.7f + rng.nextUnsignedFloat() * 0.3f;
                }
            }
            repaint();
        }

        void updateParams (float position, float spray)
        {
            currentCenterPos = position;
            currentSpray = spray;
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            g.setColour (juce::Colour::fromRGBA (20, 32, 26, 220));
            g.fillRoundedRectangle (bounds, 8.0f);

            // Center playhead indicator
            const float px = bounds.getX() + currentCenterPos * bounds.getWidth();
            g.setColour (juce::Colour::fromRGBA (118, 154, 130, 90));
            g.drawVerticalLine (static_cast<int> (px), bounds.getY(), bounds.getBottom());

            // Draw glowing grain particles
            for (const auto& gr : grains)
            {
                const float gx = bounds.getX() + gr.x * bounds.getWidth();
                const float gy = bounds.getY() + gr.y * bounds.getHeight();

                g.setColour (juce::Colour::fromRGBA (160, 230, 190, static_cast<juce::uint8> (gr.alpha * 200)));
                g.fillEllipse (gx - gr.size * 0.5f, gy - gr.size * 0.5f, gr.size, gr.size);
            }

            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (juce::Colour::fromRGBA (255, 255, 255, 120));
            g.drawText ("GRAIN CLOUD (LIVE)", bounds.getX() + 8.0f, bounds.getY() + 4.0f, 140, 14, juce::Justification::left, true);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        std::vector<DisplayGrain> grains;
        float currentCenterPos = 0.25f;
        float currentSpray = 0.15f;
        FastRandom rng;
    };
}
