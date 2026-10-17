#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>
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
            updateWaveform (0.25f);
        }

        void updateWaveform (float position)
        {
            currentPosition = juce::jlimit (0.0f, 1.0f, position);

            // Generate morphed shape for preview (Sine -> Tri -> Saw -> Formant / Complex)
            for (size_t i = 0; i < points.size(); ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (points.size());
                const float angle = t * juce::MathConstants<float>::twoPi;
                const float sinV = std::sin (angle);
                const float triV = (t < 0.25f ? 4.0f * t : (t < 0.75f ? 2.0f - 4.0f * t : 4.0f * t - 4.0f));
                const float sawV = (1.0f - 2.0f * t) * 0.85f;
                const float harmonicV = 0.6f * std::sin (angle) + 0.3f * std::sin (2.0f * angle) + 0.2f * std::sin (3.0f * angle);

                if (currentPosition < 0.33f)
                {
                    const float blend = currentPosition / 0.33f;
                    points[i] = sinV * (1.0f - blend) + triV * blend;
                }
                else if (currentPosition < 0.66f)
                {
                    const float blend = (currentPosition - 0.33f) / 0.33f;
                    points[i] = triV * (1.0f - blend) + sawV * blend;
                }
                else
                {
                    const float blend = (currentPosition - 0.66f) / 0.34f;
                    points[i] = sawV * (1.0f - blend) + harmonicV * blend;
                }
            }
            repaint();
        }

        void setMode3D (bool shouldBe3D)
        {
            is3D = shouldBe3D;
            repaint();
        }

        bool getMode3D() const noexcept { return is3D; }

        std::function<void(float)> onPositionChanged;

        void mouseDown (const juce::MouseEvent& e) override
        {
            // Check if clicking the 2D/3D toggle button in top right
            auto toggleBounds = juce::Rectangle<int> (getWidth() - 76, 8, 66, 20);
            if (toggleBounds.contains (e.getPosition()))
            {
                is3D = !is3D;
                repaint();
                return;
            }

            handleScrub (e.x);
        }

        void mouseDrag (const juce::MouseEvent& e) override
        {
            handleScrub (e.x);
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);

            // 1. Sleek Dark Canvas with soft vignette (#202428)
            juce::ColourGradient bgGrad (juce::Colour::fromRGB (0x26, 0x2B, 0x2E), bounds.getCentreX(), bounds.getY(),
                                         juce::Colour::fromRGB (0x19, 0x1C, 0x1E), bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill (bgGrad);
            g.fillRoundedRectangle (bounds, 6.0f);

            // Outline with platinum border
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.5f));
            g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

            // 2. Grid floor / perspective grid lines
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 18));
            const int gridCols = 8;
            for (int c = 1; c < gridCols; ++c)
            {
                const float gx = bounds.getX() + (bounds.getWidth() / gridCols) * c;
                g.drawVerticalLine (static_cast<int> (gx), bounds.getY() + 30.0f, bounds.getBottom() - 10.0f);
            }

            if (is3D)
                paint3DWavetable (g, bounds);
            else
                paint2DWavetable (g, bounds);

            // 3. Header Information Bar
            g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite().withAlpha (0.9f));
            g.drawText ("OSC 1 — VITAL / SERUM WAVETABLE 2048", bounds.getX() + 12.0f, bounds.getY() + 8.0f, 300, 16, juce::Justification::left, true);

            g.setFont (juce::FontOptions (10.0f));
            g.setColour (lookAndFeel.getGoldenPeach());
            const juce::String posStr = "WT POS: " + juce::String (std::round (currentPosition * 100.0f), 0) + "%";
            g.drawText (posStr, bounds.getX() + 310.0f, bounds.getY() + 8.0f, 100, 16, juce::Justification::left, true);

            // 4. 2D / 3D Mode Toggle Pill in top right
            auto toggleBounds = juce::Rectangle<float> (bounds.getRight() - 74.0f, bounds.getY() + 6.0f, 64.0f, 20.0f);
            g.setColour (lookAndFeel.getWhisperWhite().withAlpha (0.12f));
            g.fillRoundedRectangle (toggleBounds, 4.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.35f));
            g.drawRoundedRectangle (toggleBounds, 4.0f, 1.0f);

            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (is3D ? lookAndFeel.getGoldenPeach() : lookAndFeel.getPeachCoral());
            g.drawText (is3D ? "[ 3D MESH ]" : "[ 2D WAVE ]", toggleBounds, juce::Justification::centred, true);

            // 5. Position Scrubber Marker at the bottom
            const float scrubW = bounds.getWidth() - 24.0f;
            const float scrubY = bounds.getBottom() - 10.0f;
            const float scrubX = bounds.getX() + 12.0f + currentPosition * scrubW;

            // Track line
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 30));
            g.drawHorizontalLine (static_cast<int> (scrubY), bounds.getX() + 12.0f, bounds.getRight() - 12.0f);

            // Active dot
            g.setColour (lookAndFeel.getGoldenPeach());
            g.fillEllipse (scrubX - 3.5f, scrubY - 3.5f, 7.0f, 7.0f);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        std::vector<float> points;
        float currentPosition = 0.25f;
        bool is3D = true;

        void handleScrub (int mouseX)
        {
            const auto bounds = getLocalBounds().toFloat().reduced (20.0f);
            const float newPos = juce::jlimit (0.0f, 1.0f, (static_cast<float>(mouseX) - bounds.getX()) / bounds.getWidth());
            updateWaveform (newPos);
            if (onPositionChanged)
                onPositionChanged (newPos);
        }

        void paint2DWavetable (juce::Graphics& g, const juce::Rectangle<float>& bounds)
        {
            const float centerY = bounds.getY() + bounds.getHeight() * 0.54f;
            const float waveH = bounds.getHeight() * 0.36f;
            const float waveLeft = bounds.getX() + 24.0f;
            const float waveWidth = bounds.getWidth() - 48.0f;

            // Zero line
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 25));
            g.drawHorizontalLine (static_cast<int> (centerY), waveLeft, waveLeft + waveWidth);

            juce::Path wavePath;
            juce::Path fillPath;

            for (size_t i = 0; i < points.size(); ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (points.size() - 1);
                const float px = waveLeft + t * waveWidth;
                const float py = centerY - points[i] * waveH;

                if (i == 0)
                {
                    wavePath.startNewSubPath (px, py);
                    fillPath.startNewSubPath (px, centerY);
                    fillPath.lineTo (px, py);
                }
                else
                {
                    wavePath.lineTo (px, py);
                    fillPath.lineTo (px, py);
                }
            }

            fillPath.lineTo (waveLeft + waveWidth, centerY);
            fillPath.closeSubPath();

            // Gradient Underfill (Soft Peach Delight glow)
            juce::ColourGradient fillGrad (lookAndFeel.getGoldenPeach().withAlpha (0.35f), waveLeft, centerY - waveH,
                                          lookAndFeel.getPeachCoral().withAlpha (0.02f), waveLeft, centerY + waveH, false);
            g.setGradientFill (fillGrad);
            g.fillPath (fillPath);

            // Glow Stroke
            g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.35f));
            g.strokePath (wavePath, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // Sharp Neon Stroke
            g.setColour (lookAndFeel.getGoldenPeach());
            g.strokePath (wavePath, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        void paint3DWavetable (juce::Graphics& g, const juce::Rectangle<float>& bounds)
        {
            // 3D Perspective Waterfall Rendering (16 slices)
            const int numSlices = 18;
            const int activeSliceIndex = static_cast<int> (std::round (currentPosition * (numSlices - 1)));

            const float startY = bounds.getY() + 34.0f;
            const float totalDepthY = bounds.getHeight() * 0.52f;
            const float sliceSpacingY = totalDepthY / static_cast<float> (numSlices);

            // Draw from back to front (painter's algorithm)
            for (int s = numSlices - 1; s >= 0; --s)
            {
                const float depthFactor = static_cast<float> (s) / static_cast<float> (numSlices - 1); // 1.0 = front, 0.0 = back
                const float slicePos = static_cast<float> (s) / static_cast<float> (numSlices - 1);

                // Perspective projection factors
                const float perspectiveScale = 0.72f + depthFactor * 0.28f;
                const float sliceWidth = (bounds.getWidth() - 64.0f) * perspectiveScale;
                const float sliceLeft = bounds.getCentreX() - sliceWidth * 0.5f;
                const float sliceY = startY + (1.0f - depthFactor) * totalDepthY;
                const float sliceHeight = (bounds.getHeight() * 0.22f) * perspectiveScale;

                const bool isActive = (s == activeSliceIndex);

                // Calculate wave points for slice s
                juce::Path slicePath;
                juce::Path fillPath;

                const int numPts = 64;
                for (int i = 0; i < numPts; ++i)
                {
                    const float t = static_cast<float> (i) / static_cast<float> (numPts - 1);
                    const float angle = t * juce::MathConstants<float>::twoPi;

                    // Blend morph for this slice
                    const float sinV = std::sin (angle);
                    const float triV = (t < 0.25f ? 4.0f * t : (t < 0.75f ? 2.0f - 4.0f * t : 4.0f * t - 4.0f));
                    const float sawV = (1.0f - 2.0f * t) * 0.85f;
                    const float harmV = 0.6f * std::sin (angle) + 0.35f * std::sin (2.0f * angle);

                    float v = 0.0f;
                    if (slicePos < 0.33f)
                        v = sinV * (1.0f - slicePos / 0.33f) + triV * (slicePos / 0.33f);
                    else if (slicePos < 0.66f)
                        v = triV * (1.0f - (slicePos - 0.33f) / 0.33f) + sawV * ((slicePos - 0.33f) / 0.33f);
                    else
                        v = sawV * (1.0f - (slicePos - 0.66f) / 0.34f) + harmV * ((slicePos - 0.66f) / 0.34f);

                    const float px = sliceLeft + t * sliceWidth;
                    const float py = sliceY - v * sliceHeight;

                    if (i == 0)
                    {
                        slicePath.startNewSubPath (px, py);
                        fillPath.startNewSubPath (px, sliceY);
                        fillPath.lineTo (px, py);
                    }
                    else
                    {
                        slicePath.lineTo (px, py);
                        fillPath.lineTo (px, py);
                    }
                }

                fillPath.lineTo (sliceLeft + sliceWidth, sliceY);
                fillPath.closeSubPath();

                // Fill under slice to occlude slices behind
                g.setColour (juce::Colour::fromRGB (0x1C, 0x20, 0x22));
                g.fillPath (fillPath);

                if (isActive)
                {
                    // Active Slice Highlight (Vibrant Golden Peach #FEC89A + Coral Glow #FEC5BB)
                    juce::ColourGradient activeFill (lookAndFeel.getGoldenPeach().withAlpha (0.42f), sliceLeft, sliceY - sliceHeight,
                                                    lookAndFeel.getPeachCoral().withAlpha (0.05f), sliceLeft, sliceY, false);
                    g.setGradientFill (activeFill);
                    g.fillPath (fillPath);

                    g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.6f));
                    g.strokePath (slicePath, juce::PathStrokeType (3.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

                    g.setColour (lookAndFeel.getGoldenPeach());
                    g.strokePath (slicePath, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
                }
                else
                {
                    // Inactive slices: Soft pastel eucalyptus sage & faint pink
                    const float alpha = 0.25f + depthFactor * 0.45f;
                    juce::Colour sliceCol = lookAndFeel.getSageMint().interpolatedWith (lookAndFeel.getBlushPink(), depthFactor);
                    g.setColour (sliceCol.withAlpha (alpha));
                    g.strokePath (slicePath, juce::PathStrokeType (1.0f));
                }
            }
        }
    };
}
