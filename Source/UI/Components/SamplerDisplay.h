#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    class SamplerDisplay : public juce::Component
    {
    public:
        SamplerDisplay (BotanicaLookAndFeel& laf)
            : lookAndFeel (laf)
        {
            sampleName = "nature vol. i - birds forest 4";
        }

        void setSampleName (const juce::String& name)
        {
            sampleName = name;
            repaint();
        }

        void setWaveform (const std::vector<float>& newWaveform)
        {
            waveform = newWaveform;
            repaint();
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);

            // Dark recessed background (#202428)
            g.setColour (juce::Colour::fromRGB (0x20, 0x24, 0x28));
            g.fillRoundedRectangle (bounds, 6.0f);

            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.45f));
            g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

            // Draw grid lines
            const int gridCols = 16;
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 14));
            for (int i = 1; i < gridCols; ++i)
            {
                const float x = bounds.getX() + (bounds.getWidth() / gridCols) * i;
                g.drawVerticalLine (static_cast<int>(x), bounds.getY() + 4.0f, bounds.getBottom() - 4.0f);
            }

            // Horizontal center line
            const float centerY = bounds.getCentreY();
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 22));
            g.drawHorizontalLine (static_cast<int>(centerY), bounds.getX() + 28.0f, bounds.getRight() - 28.0f);

            // Left velocity markers matching design1.png (000, 063, 127)
            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.6f));
            g.drawText ("000", bounds.getX() + 6.0f, bounds.getY() + 10.0f, 24, 14, juce::Justification::left, true);
            g.drawText ("063", bounds.getX() + 6.0f, centerY - 7.0f, 24, 14, juce::Justification::left, true);
            g.drawText ("127", bounds.getX() + 6.0f, bounds.getBottom() - 24.0f, 24, 14, juce::Justification::left, true);

            // Right velocity markers
            g.drawText ("000", bounds.getRight() - 30.0f, bounds.getY() + 10.0f, 24, 14, juce::Justification::right, true);
            g.drawText ("063", bounds.getRight() - 30.0f, centerY - 7.0f, 24, 14, juce::Justification::right, true);
            g.drawText ("127", bounds.getRight() - 30.0f, bounds.getBottom() - 24.0f, 24, 14, juce::Justification::right, true);

            // Draw Sample Waveform
            const float waveLeft = bounds.getX() + 32.0f;
            const float waveWidth = bounds.getWidth() - 64.0f;
            const float waveH = bounds.getHeight() * 0.42f;

            if (!waveform.empty())
            {
                juce::Path wavePath;
                juce::Path fillPath;

                for (size_t i = 0; i < waveform.size(); ++i)
                {
                    const float t = static_cast<float> (i) / static_cast<float> (waveform.size() - 1);
                    const float x = waveLeft + t * waveWidth;
                    const float y = centerY - waveform[i] * waveH;

                    if (i == 0)
                    {
                        wavePath.startNewSubPath (x, y);
                        fillPath.startNewSubPath (x, centerY);
                        fillPath.lineTo (x, y);
                    }
                    else
                    {
                        wavePath.lineTo (x, y);
                        fillPath.lineTo (x, y);
                    }
                }
                fillPath.lineTo (waveLeft + waveWidth, centerY);
                fillPath.closeSubPath();

                // Glow underfill
                g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.18f));
                g.fillPath (fillPath);

                g.setColour (lookAndFeel.getGoldenPeach().withAlpha (0.75f));
                g.strokePath (wavePath, juce::PathStrokeType (1.8f));
            }
            else
            {
                // Default subtle ambient wave preview
                juce::Path wavePath;
                const int numPts = 120;
                for (int i = 0; i < numPts; ++i)
                {
                    const float t = static_cast<float> (i) / static_cast<float> (numPts - 1);
                    const float x = waveLeft + t * waveWidth;
                    const float env = std::sin (t * juce::MathConstants<float>::pi);
                    const float y = centerY - std::sin (t * 18.0f) * env * waveH * 0.45f;

                    if (i == 0) wavePath.startNewSubPath (x, y);
                    else wavePath.lineTo (x, y);
                }
                g.setColour (lookAndFeel.getSageMint().withAlpha (0.35f));
                g.strokePath (wavePath, juce::PathStrokeType (1.4f));
            }

            // Center Sample Label matching design1.png ("nature vol. i - birds forest 4")
            g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite().withAlpha (0.85f));
            g.drawText (sampleName, bounds, juce::Justification::centred, true);

            // Drag and drop hint
            g.setFont (juce::FontOptions (10.0f));
            g.setColour (lookAndFeel.getGoldenPeach().withAlpha (0.7f));
            g.drawText ("DROP AUDIO FILE HERE TO LOAD SAMPLE", bounds.getX(), bounds.getBottom() - 24.0f, bounds.getWidth(), 16, juce::Justification::centred, true);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::String sampleName;
        std::vector<float> waveform;
    };
}
