#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"
#include <cmath>

namespace Floweret
{
    //==============================================================================
    // Real-Time Filter Frequency Response Curve Visualizer
    //==============================================================================
    class FilterResponseCurve : public juce::Component
    {
    public:
        FilterResponseCurve (BotanicaLookAndFeel& laf)
            : lookAndFeel (laf)
        {
        }

        void updateFilter (float cutoffHz, float resonanceVal, int typeIdx)
        {
            cutoff = juce::jlimit (20.0f, 20000.0f, cutoffHz);
            resonance = juce::jlimit (0.1f, 10.0f, resonanceVal);
            filterType = typeIdx;
            repaint();
        }

        std::function<void(float cutoff, float res)> onFilterChanged;

        void mouseDown (const juce::MouseEvent& e) override { handleDrag (e.getPosition()); }
        void mouseDrag (const juce::MouseEvent& e) override { handleDrag (e.getPosition()); }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // 1. Dark Screen Background (#1E2224)
            g.setColour (juce::Colour::fromRGB (0x1E, 0x22, 0x24));
            g.fillRoundedRectangle (bounds, 5.0f);

            // Platinum border
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.4f));
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            const float w = bounds.getWidth() - 8.0f;
            const float h = bounds.getHeight() - 8.0f;
            const float startX = bounds.getX() + 4.0f;
            const float startY = bounds.getY() + 4.0f;

            // 2. Frequency Grid Lines (100 Hz, 1 kHz, 10 kHz)
            auto freqToX = [&](float f) -> float {
                const float norm = std::log10 (juce::jlimit (20.0f, 20000.0f, f) / 20.0f) / std::log10 (1000.0f);
                return startX + norm * w;
            };

            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 18));
            g.drawVerticalLine (static_cast<int>(freqToX (100.0f)), startY, startY + h);
            g.drawVerticalLine (static_cast<int>(freqToX (1000.0f)), startY, startY + h);
            g.drawVerticalLine (static_cast<int>(freqToX (10000.0f)), startY, startY + h);

            // Frequency Labels
            g.setFont (juce::FontOptions (8.5f));
            g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.6f));
            g.drawText ("100", freqToX (100.0f) + 2.0f, startY + 2.0f, 24, 10, juce::Justification::left, true);
            g.drawText ("1k",  freqToX (1000.0f) + 2.0f, startY + 2.0f, 24, 10, juce::Justification::left, true);
            g.drawText ("10k", freqToX (10000.0f) + 2.0f, startY + 2.0f, 24, 10, juce::Justification::left, true);

            // 0 dB center reference line
            const float zeroDbY = startY + h * 0.45f;
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 25));
            g.drawHorizontalLine (static_cast<int>(zeroDbY), startX, startX + w);

            // 3. Compute Real-Time Filter Magnitude Curve
            juce::Path curve;
            juce::Path fill;

            const int numPoints = 120;
            const float pixelsPerDb = h / 48.0f; // 48 dB total range (-32 dB to +16 dB)

            for (int i = 0; i < numPoints; ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (numPoints - 1);
                const float f = 20.0f * std::pow (1000.0f, t);
                const float px = startX + t * w;

                const float r = f / cutoff;
                float gain = 1.0f;

                // Transfer function magnitude calculation
                switch (filterType)
                {
                    case 0: // LP 12dB
                        gain = 1.0f / std::sqrt (std::pow (1.0f - r * r, 2.0f) + std::pow (r / resonance, 2.0f));
                        break;
                    case 1: // LP 24dB
                    default:
                    {
                        const float single = 1.0f / std::sqrt (std::pow (1.0f - r * r, 2.0f) + std::pow (r / resonance, 2.0f));
                        gain = single * single;
                        break;
                    }
                    case 2: // HP 12dB
                        gain = (r * r) / std::sqrt (std::pow (1.0f - r * r, 2.0f) + std::pow (r / resonance, 2.0f));
                        break;
                    case 3: // HP 24dB
                    {
                        const float single = (r * r) / std::sqrt (std::pow (1.0f - r * r, 2.0f) + std::pow (r / resonance, 2.0f));
                        gain = single * single;
                        break;
                    }
                    case 4: // BandPass
                        gain = (r / resonance) / std::sqrt (std::pow (1.0f - r * r, 2.0f) + std::pow (r / resonance, 2.0f));
                        break;
                    case 5: // Notch
                        gain = std::abs (1.0f - r * r) / std::sqrt (std::pow (1.0f - r * r, 2.0f) + std::pow (r / resonance, 2.0f));
                        break;
                    case 6: // Comb
                        gain = std::abs (std::cos (3.0f * juce::MathConstants<float>::pi * r)) * (1.0f / std::sqrt (1.0f + r * r));
                        break;
                    case 7: // Formant
                    {
                        const float r1 = f / (cutoff * 0.7f);
                        const float r2 = f / (cutoff * 1.5f);
                        const float g1 = 1.0f / std::sqrt (std::pow (1.0f - r1 * r1, 2.0f) + std::pow (r1 / 4.0f, 2.0f));
                        const float g2 = 0.7f / std::sqrt (std::pow (1.0f - r2 * r2, 2.0f) + std::pow (r2 / 4.0f, 2.0f));
                        gain = g1 + g2;
                        break;
                    }
                }

                const float db = 20.0f * std::log10 (juce::jmax (0.001f, gain));
                const float py = juce::jlimit (startY + 2.0f, startY + h - 2.0f, zeroDbY - db * pixelsPerDb);

                if (i == 0)
                {
                    curve.startNewSubPath (px, py);
                    fill.startNewSubPath (px, startY + h);
                    fill.lineTo (px, py);
                }
                else
                {
                    curve.lineTo (px, py);
                    fill.lineTo (px, py);
                }
            }

            fill.lineTo (startX + w, startY + h);
            fill.closeSubPath();

            // Gradient Underfill matching reference image (cyan/blue & peachy coral)
            juce::ColourGradient fillGrad (juce::Colour::fromRGBA (45, 156, 219, 140), startX, startY,
                                          juce::Colour::fromRGBA (254, 197, 187, 25), startX, startY + h, false);
            g.setGradientFill (fillGrad);
            g.fillPath (fill);

            // Glowing curve stroke
            g.setColour (juce::Colour::fromRGBA (56, 182, 255, 90));
            g.strokePath (curve, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            g.setColour (juce::Colour::fromRGB (56, 182, 255));
            g.strokePath (curve, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // 4. Interactive Cutoff / Resonance Puck on curve
            const float puckX = freqToX (cutoff);
            const float puckDb = 20.0f * std::log10 (juce::jmax (0.001f, resonance));
            const float puckY = juce::jlimit (startY + 4.0f, startY + h - 4.0f, zeroDbY - puckDb * pixelsPerDb);

            // Puck shadow & outer ring
            g.setColour (juce::Colour::fromRGBA (56, 182, 255, 120));
            g.fillEllipse (puckX - 6.0f, puckY - 6.0f, 12.0f, 12.0f);
            g.setColour (juce::Colour::fromRGB (0xFF, 0xFF, 0xFF));
            g.fillEllipse (puckX - 3.5f, puckY - 3.5f, 7.0f, 7.0f);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        float cutoff = 2500.0f;
        float resonance = 1.5f;
        int filterType = 1;

        void handleDrag (juce::Point<int> pos)
        {
            const auto bounds = getLocalBounds().toFloat().reduced (4.0f);
            const float normX = juce::jlimit (0.0f, 1.0f, (static_cast<float>(pos.x) - bounds.getX()) / bounds.getWidth());
            const float newCutoff = 20.0f * std::pow (1000.0f, normX);

            const float zeroDbY = bounds.getY() + bounds.getHeight() * 0.45f;
            const float normY = (zeroDbY - static_cast<float>(pos.y)) / (bounds.getHeight() * 0.45f);
            const float newRes = juce::jlimit (0.1f, 10.0f, std::pow (10.0f, normY));

            updateFilter (newCutoff, newRes, filterType);
            if (onFilterChanged)
                onFilterChanged (newCutoff, newRes);
        }
    };

    //==============================================================================
    // PreFilterModule (Merged wide box with 3 tabs F1-F3 and real-time visualization)
    //==============================================================================
    class PreFilterModule : public juce::Component
    {
    public:
        PreFilterModule (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf), visualizer (laf)
        {
            // Title
            titleLabel.setText ("PRE FILTER", juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            // Power button
            powerBtn.setButtonText ("I");
            powerBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (powerBtn);

            // 3 Tabs (F1, F2, F3) as requested
            tabF1.setButtonText ("F1");
            tabF2.setButtonText ("F2");
            tabF3.setButtonText ("F3");
            tabF1.setToggleState (true, juce::dontSendNotification);

            addAndMakeVisible (tabF1);
            addAndMakeVisible (tabF2);
            addAndMakeVisible (tabF3);

            tabF1.onClick = [this]() { selectFilterTab (0); };
            tabF2.onClick = [this]() { selectFilterTab (1); };
            tabF3.onClick = [this]() { selectFilterTab (2); };

            // Filter Type Selector Bar with < and > chevrons matching screenshot
            prevTypeBtn.setButtonText ("<");
            nextTypeBtn.setButtonText (">");
            addAndMakeVisible (prevTypeBtn);
            addAndMakeVisible (nextTypeBtn);

            typeBox.addItemList (getFilterTypeNames(), 1);
            typeBox.setSelectedId (2, juce::dontSendNotification); // LP 24dB
            addAndMakeVisible (typeBox);

            prevTypeBtn.onClick = [this]() {
                int id = typeBox.getSelectedId() - 1;
                if (id < 1) id = typeBox.getNumItems();
                typeBox.setSelectedId (id);
            };

            nextTypeBtn.onClick = [this]() {
                int id = typeBox.getSelectedId() + 1;
                if (id > typeBox.getNumItems()) id = 1;
                typeBox.setSelectedId (id);
            };

            // Dials
            setupDial (cutoffSlider, cutoffLabel, "CUTOFF");
            setupDial (resSlider, resLabel, "RESONANCE");
            setupDial (driveSlider, driveLabel, "DRIVE");
            setupDial (mixSlider, mixLabel, "KEYTRK");

            mixSlider.setRange (0.0, 1.0, 0.01);
            mixSlider.setValue (0.5);

            // Real-time curve updates
            auto updateVisualizer = [this]() {
                visualizer.updateFilter (static_cast<float> (cutoffSlider.getValue()),
                                        static_cast<float> (resSlider.getValue()),
                                        typeBox.getSelectedId() - 1);
            };

            cutoffSlider.onValueChange = updateVisualizer;
            resSlider.onValueChange    = updateVisualizer;
            typeBox.onChange           = updateVisualizer;

            // Puck drag on visualizer updates sliders
            visualizer.onFilterChanged = [this](float c, float r) {
                cutoffSlider.setValue (c, juce::dontSendNotification);
                resSlider.setValue (r, juce::dontSendNotification);
            };

            addAndMakeVisible (visualizer);
            updateVisualizer();

            // APVTS Attachments
            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, ParamIDs::filterType.getParamID(), typeBox));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::filterCutoff.getParamID(), cutoffSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::filterResonance.getParamID(), resSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::filterDrive.getParamID(), driveSlider));
        }

        ~PreFilterModule() override = default;

        void selectFilterTab (int index)
        {
            activeTab = index;
            tabF1.setToggleState (activeTab == 0, juce::dontSendNotification);
            tabF2.setToggleState (activeTab == 1, juce::dontSendNotification);
            tabF3.setToggleState (activeTab == 2, juce::dontSendNotification);

            titleLabel.setText ("PRE FILTER " + juce::String (activeTab + 1), juce::dontSendNotification);

            // Provide varied default curve for F2 and F3 preview
            if (activeTab == 1)
            {
                visualizer.updateFilter (4500.0f, 2.5f, 0); // F2
            }
            else if (activeTab == 2)
            {
                visualizer.updateFilter (800.0f, 3.2f, 4); // F3 BandPass
            }
            else
            {
                visualizer.updateFilter (static_cast<float> (cutoffSlider.getValue()),
                                        static_cast<float> (resSlider.getValue()),
                                        typeBox.getSelectedId() - 1);
            }
            repaint();
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // Card background (#F8EDEB)
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (bounds, 5.0f);

            // Platinum border
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            // Header line
            g.drawHorizontalLine (24, bounds.getX(), bounds.getRight());
        }

        void resized() override
        {
            const auto bounds = getLocalBounds().reduced (3);

            // Total width split: Left side (controls, ~240px) and Right side (visualization, ~260px)
            const int leftW = (bounds.getWidth() - 8) * 48 / 100;
            const int rightW = bounds.getWidth() - leftW - 8;
            const int rightX = bounds.getX() + leftW + 8;

            // 1. Left Section Header (Power, Title, F1 - F3 tabs)
            powerBtn.setBounds   (bounds.getX() + 4, bounds.getY() + 3, 18, 18);
            titleLabel.setBounds (bounds.getX() + 26, bounds.getY() + 3, 76, 18);

            const int tabBtnW = 26;
            tabF1.setBounds (bounds.getX() + 104, bounds.getY() + 3, tabBtnW, 18);
            tabF2.setBounds (bounds.getX() + 132, bounds.getY() + 3, tabBtnW, 18);
            tabF3.setBounds (bounds.getX() + 160, bounds.getY() + 3, tabBtnW, 18);

            // Filter Type Selector Bar with < and > chevrons
            const int typeY = bounds.getY() + 28;
            prevTypeBtn.setBounds (bounds.getX() + 4, typeY, 20, 20);
            typeBox.setBounds     (bounds.getX() + 26, typeY, leftW - 52, 20);
            nextTypeBtn.setBounds (bounds.getX() + leftW - 24, typeY, 20, 20);

            // 4 Dials (arranged in 2x2 grid or row)
            const int dialY = bounds.getY() + 54;
            const int dialW = (leftW - 12) / 4;
            const int dialSize = juce::jmin (dialW - 4, 38);

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = bounds.getX() + 4 + idx * dialW;
                l.setBounds (dx, dialY, dialW, 13);
                s.setBounds (dx + (dialW - dialSize) / 2, dialY + 15, dialSize, dialSize);
            };

            placeDial (cutoffSlider, cutoffLabel, 0);
            placeDial (resSlider, resLabel, 1);
            placeDial (driveSlider, driveLabel, 2);
            placeDial (mixSlider, mixLabel, 3);

            // 2. Right Section: Real-Time Frequency Response Visualizer
            visualizer.setBounds (rightX, bounds.getY() + 2, rightW - 2, bounds.getHeight() - 4);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        int activeTab = 0;

        juce::TextButton powerBtn;
        juce::Label titleLabel;
        juce::TextButton tabF1, tabF2, tabF3;

        juce::TextButton prevTypeBtn, nextTypeBtn;
        juce::ComboBox typeBox;

        juce::Slider cutoffSlider, resSlider, driveSlider, mixSlider;
        juce::Label cutoffLabel, resLabel, driveLabel, mixLabel;

        FilterResponseCurve visualizer;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;

        void setupDial (juce::Slider& s, juce::Label& l, const juce::String& text)
        {
            l.setText (text, juce::dontSendNotification);
            l.setFont (juce::FontOptions (8.5f, juce::Font::bold));
            l.setColour (juce::Label::textColourId, lookAndFeel.getMutedSlate());
            l.setJustificationType (juce::Justification::centred);
            addAndMakeVisible (l);

            s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            s.setLookAndFeel (&lookAndFeel);
            addAndMakeVisible (s);
        }
    };
}
