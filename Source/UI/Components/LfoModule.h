#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    class LfoModule : public juce::Component
    {
    public:
        LfoModule (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            titleLabel.setText ("LFO (4 LFO Default)", juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            tabLfo1.setButtonText ("1");
            tabLfo2.setButtonText ("2");
            tabLfo3.setButtonText ("3");
            tabLfo4.setButtonText ("4");
            tabLfo1.setToggleState (true, juce::dontSendNotification);

            addAndMakeVisible (tabLfo1);
            addAndMakeVisible (tabLfo2);
            addAndMakeVisible (tabLfo3);
            addAndMakeVisible (tabLfo4);

            shapeBox.addItemList ({ "Sine", "Triangle", "Saw Down", "Saw Up", "Square", "Random S&H" }, 1);
            shapeBox.setSelectedId (1, juce::dontSendNotification);
            addAndMakeVisible (shapeBox);

            setupDial (rateSlider, rateLabel, "RATE");
            setupDial (phaseSlider, phaseLabel, "PHASE");
            setupDial (chaosSlider, chaosLabel, "CHAOS");
            setupDial (smoothSlider, smoothLabel, "SMOOTH");

            shapeBox.onChange = [this]() { repaint(); };
            rateSlider.onValueChange = [this]() { repaint(); };

            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, ParamIDs::lfoShape.getParamID(), shapeBox));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::lfoRate.getParamID(), rateSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::lfoChaos.getParamID(), chaosSlider));
        }

        ~LfoModule() override = default;

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // Card background (#F8EDEB)
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (bounds, 5.0f);

            // Border
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            // Header line
            g.drawHorizontalLine (24, bounds.getX(), bounds.getRight());

            // LFO Wave Display Box (y: 28 to 110)
            auto waveBounds = juce::Rectangle<float> (bounds.getX() + 8.0f, 28.0f, bounds.getWidth() - 16.0f, 86.0f);
            g.setColour (juce::Colour::fromRGB (0x22, 0x26, 0x28));
            g.fillRoundedRectangle (waveBounds, 4.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.35f));
            g.drawRoundedRectangle (waveBounds, 4.0f, 1.0f);

            // Grid line
            g.setColour (juce::Colour::fromRGBA (232, 232, 228, 20));
            g.drawHorizontalLine (static_cast<int>(waveBounds.getCentreY()), waveBounds.getX() + 4.0f, waveBounds.getRight() - 4.0f);

            // Draw LFO Waveform
            juce::Path wave;
            juce::Path fill;

            const float w = waveBounds.getWidth() - 16.0f;
            const float h = waveBounds.getHeight() * 0.38f;
            const float startX = waveBounds.getX() + 8.0f;
            const float midY = waveBounds.getCentreY();

            const int shapeId = shapeBox.getSelectedId() - 1; // 0: Sine, 1: Tri, 2: SawDown, 3: SawUp, 4: Square, 5: S&H
            const int pts = 100;

            for (int i = 0; i < pts; ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (pts - 1);
                float val = 0.0f;

                if (shapeId == 0) // Sine
                    val = std::sin (t * juce::MathConstants<float>::twoPi * 2.0f);
                else if (shapeId == 1) // Tri
                {
                    const float ph = std::fmod (t * 2.0f, 1.0f);
                    val = ph < 0.5f ? 4.0f * ph - 1.0f : 3.0f - 4.0f * ph;
                }
                else if (shapeId == 2) // Saw Down
                    val = 1.0f - 2.0f * std::fmod (t * 2.0f, 1.0f);
                else if (shapeId == 3) // Saw Up
                    val = 2.0f * std::fmod (t * 2.0f, 1.0f) - 1.0f;
                else if (shapeId == 4) // Square
                    val = std::fmod (t * 2.0f, 1.0f) < 0.5f ? 0.9f : -0.9f;
                else // S&H
                    val = std::sin (std::floor (t * 8.0f) * 1.57f);

                const float px = startX + t * w;
                const float py = midY - val * h;

                if (i == 0)
                {
                    wave.startNewSubPath (px, py);
                    fill.startNewSubPath (px, midY);
                    fill.lineTo (px, py);
                }
                else
                {
                    wave.lineTo (px, py);
                    fill.lineTo (px, py);
                }
            }

            fill.lineTo (startX + w, midY);
            fill.closeSubPath();

            // Gradient fill
            juce::ColourGradient fillGrad (lookAndFeel.getGoldenPeach().withAlpha (0.35f), startX, midY - h,
                                          lookAndFeel.getPeachCoral().withAlpha (0.04f), startX, midY + h, false);
            g.setGradientFill (fillGrad);
            g.fillPath (fill);

            // Glow & line
            g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.4f));
            g.strokePath (wave, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            g.setColour (lookAndFeel.getGoldenPeach());
            g.strokePath (wave, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        void resized() override
        {
            titleLabel.setBounds (8, 4, 130, 18);
            tabLfo1.setBounds (144, 4, 22, 18);
            tabLfo2.setBounds (168, 4, 22, 18);
            tabLfo3.setBounds (192, 4, 22, 18);
            tabLfo4.setBounds (216, 4, 22, 18);

            shapeBox.setBounds (getWidth() - 118, 3, 110, 20);

            // Dials below display (y: 122 to bottom)
            const int dialY = 138;
            const int dialW = (getWidth() - 16) / 4;
            const int dialSize = juce::jmin (dialW - 4, 52);

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = 8 + idx * dialW;
                l.setBounds (dx, 120, dialW, 14);
                s.setBounds (dx + (dialW - dialSize) / 2, dialY, dialSize, dialSize);
            };

            placeDial (rateSlider, rateLabel, 0);
            placeDial (phaseSlider, phaseLabel, 1);
            placeDial (chaosSlider, chaosLabel, 2);
            placeDial (smoothSlider, smoothLabel, 3);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::Label titleLabel;
        juce::TextButton tabLfo1, tabLfo2, tabLfo3, tabLfo4;
        juce::ComboBox shapeBox;

        juce::Slider rateSlider, phaseSlider, chaosSlider, smoothSlider;
        juce::Label rateLabel, phaseLabel, chaosLabel, smoothLabel;

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
