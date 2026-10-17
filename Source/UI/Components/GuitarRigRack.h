#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    class GuitarRigRack : public juce::Component
    {
    public:
        GuitarRigRack (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            // Tube Compressor / Saturator Knobs
            setupKnob (distDriveKnob, "DRIVE", apvts, ParamIDs::distDrive.getParamID());
            setupKnob (distMixKnob, "MIX", apvts, ParamIDs::distMix.getParamID());

            // Color Resonator Knobs
            setupKnob (colorResKnob, "RESONANCE", apvts, ParamIDs::colorResonance.getParamID());
            setupKnob (colorMorphKnob, "MORPH", apvts, ParamIDs::spectralMorph.getParamID());
            setupKnob (colorGateKnob, "GATE", apvts, ParamIDs::spectralGate.getParamID());
            setupKnob (colorAmountKnob, "AMOUNT", apvts, ParamIDs::colorAmount.getParamID());

            // Delay Knobs
            setupKnob (delayTimeKnob, "TIME", apvts, ParamIDs::delayTime.getParamID());
            setupKnob (delayFbKnob, "FEEDBACK", apvts, ParamIDs::delayFeedback.getParamID());
            setupKnob (delayMixKnob, "MIX", apvts, ParamIDs::delayMix.getParamID());

            // Reverb Knobs
            setupKnob (reverbSizeKnob, "SIZE", apvts, ParamIDs::reverbSize.getParamID());
            setupKnob (reverbDampKnob, "DAMP", apvts, ParamIDs::reverbDamp.getParamID());
            setupKnob (reverbMixKnob, "MIX", apvts, ParamIDs::reverbMix.getParamID());
            setupKnob (bloomKnob, "BLOOM", apvts, ParamIDs::bloomAmount.getParamID());
        }

        ~GuitarRigRack() override = default;

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            const float w = bounds.getWidth();
            const float h = bounds.getHeight();

            // Background canvas (#23272A)
            g.setColour (juce::Colour::fromRGB (0x23, 0x27, 0x2A));
            g.fillRoundedRectangle (bounds, 6.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.45f));
            g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

            // Three column division matching Guitar Rig:
            // Column 1: Component browser (left 170px)
            // Column 2: Stacked Rack modules (middle)
            // Column 3: Signal Flow (right 170px)
            const float col1W = 160.0f;
            const float col3W = 170.0f;
            const float col2W = w - col1W - col3W - 8.0f;

            const auto col1Bounds = juce::Rectangle<float> (bounds.getX(), bounds.getY(), col1W, h);
            const auto col2Bounds = juce::Rectangle<float> (col1Bounds.getRight() + 4.0f, bounds.getY(), col2W, h);
            const auto col3Bounds = juce::Rectangle<float> (col2Bounds.getRight() + 4.0f, bounds.getY(), col3W, h);

            // Dividers
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.25f));
            g.drawVerticalLine (static_cast<int>(col1Bounds.getRight()), bounds.getY(), bounds.getBottom());
            g.drawVerticalLine (static_cast<int>(col3Bounds.getX()), bounds.getY(), bounds.getBottom());

            // Paint Column 1: Components Browser
            paintComponentBrowser (g, col1Bounds);

            // Paint Column 2: Stacked Rack Units
            paintRackModules (g, col2Bounds);

            // Paint Column 3: Signal Flow Diagram
            paintSignalFlow (g, col3Bounds);
        }

        void resized() override
        {
            const auto bounds = getLocalBounds().reduced (4);
            const int col1W = 160;
            const int col3W = 170;
            const int col2X = bounds.getX() + col1W + 8;
            const int col2W = bounds.getWidth() - col1W - col3W - 16;
            const int unitH = bounds.getHeight() / 4;

            // Unit 1: Tube Compressor / Saturator
            const int u1Y = bounds.getY() + 4;
            distDriveKnob.setBounds (col2X + 220, u1Y + 12, 50, 52);
            distMixKnob.setBounds   (col2X + 290, u1Y + 12, 50, 52);

            // Unit 2: Color Resonator
            const int u2Y = u1Y + unitH;
            colorAmountKnob.setBounds (col2X + 180, u2Y + 12, 50, 52);
            colorResKnob.setBounds    (col2X + 245, u2Y + 12, 50, 52);
            colorMorphKnob.setBounds  (col2X + 310, u2Y + 12, 50, 52);
            colorGateKnob.setBounds   (col2X + 375, u2Y + 12, 50, 52);

            // Unit 3: Delay
            const int u3Y = u2Y + unitH;
            delayTimeKnob.setBounds (col2X + 200, u3Y + 12, 50, 52);
            delayFbKnob.setBounds   (col2X + 270, u3Y + 12, 50, 52);
            delayMixKnob.setBounds  (col2X + 340, u3Y + 12, 50, 52);

            // Unit 4: Reverb
            const int u4Y = u3Y + unitH;
            reverbSizeKnob.setBounds (col2X + 170, u4Y + 12, 50, 52);
            reverbDampKnob.setBounds (col2X + 235, u4Y + 12, 50, 52);
            bloomKnob.setBounds      (col2X + 300, u4Y + 12, 50, 52);
            reverbMixKnob.setBounds  (col2X + 365, u4Y + 12, 50, 52);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;

        struct LabeledKnob : public juce::Component
        {
            juce::Slider slider;
            juce::Label label;

            void resized() override
            {
                label.setBounds (0, 0, getWidth(), 14);
                slider.setBounds (0, 14, getWidth(), getHeight() - 14);
            }
        };

        LabeledKnob distDriveKnob, distMixKnob;
        LabeledKnob colorResKnob, colorMorphKnob, colorGateKnob, colorAmountKnob;
        LabeledKnob delayTimeKnob, delayFbKnob, delayMixKnob;
        LabeledKnob reverbSizeKnob, reverbDampKnob, reverbMixKnob, bloomKnob;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

        void setupKnob (LabeledKnob& knob, const juce::String& text,
                        juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId)
        {
            knob.label.setText (text, juce::dontSendNotification);
            knob.label.setFont (juce::FontOptions (9.0f, juce::Font::bold));
            knob.label.setColour (juce::Label::textColourId, lookAndFeel.getGoldenPeach());
            knob.label.setJustificationType (juce::Justification::centred);
            knob.addAndMakeVisible (knob.label);

            knob.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            knob.slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            knob.slider.setLookAndFeel (&lookAndFeel);
            knob.addAndMakeVisible (knob.slider);

            addAndMakeVisible (knob);
            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, paramId, knob.slider));
        }

        void paintComponentBrowser (juce::Graphics& g, const juce::Rectangle<float>& b)
        {
            // Header
            g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite());
            g.drawText ("EFFECTS RACK", b.getX() + 10.0f, b.getY() + 8.0f, b.getWidth() - 20.0f, 18.0f, juce::Justification::left, true);

            // Category dots & search bar
            auto searchBox = juce::Rectangle<float> (b.getX() + 8.0f, b.getY() + 32.0f, b.getWidth() - 16.0f, 20.0f);
            g.setColour (juce::Colour::fromRGB (0x1B, 0x1E, 0x20));
            g.fillRoundedRectangle (searchBox, 3.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.3f));
            g.drawRoundedRectangle (searchBox, 3.0f, 1.0f);

            g.setFont (juce::FontOptions (10.0f));
            g.setColour (lookAndFeel.getMutedSlate());
            g.drawText ("Search Components...", searchBox.reduced (6.0f, 0.0f), juce::Justification::centredLeft, true);

            // Component list items matching Guitar Rig
            const juce::StringArray items = {
                "Tube Saturator", "Color Resonator", "Analog Delay", "Botanica Reverb",
                "Formant Shifter", "Velvet Noise", "Multiband Splitter", "Stereo Limiter"
            };

            const float itemY = b.getY() + 60.0f;
            const float itemH = 22.0f;
            for (int i = 0; i < items.size(); ++i)
            {
                const float y = itemY + i * itemH;
                if (i < 4)
                {
                    // Active module indicator
                    g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.15f));
                    g.fillRoundedRectangle (b.getX() + 6.0f, y, b.getWidth() - 12.0f, itemH - 2.0f, 3.0f);
                    g.setColour (lookAndFeel.getGoldenPeach());
                    g.fillEllipse (b.getX() + 12.0f, y + 6.0f, 6.0f, 6.0f);
                    g.setColour (lookAndFeel.getWhisperWhite());
                }
                else
                {
                    g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.4f));
                    g.fillEllipse (b.getX() + 12.0f, y + 6.0f, 6.0f, 6.0f);
                    g.setColour (lookAndFeel.getMutedSlate());
                }

                g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
                g.drawText (items[i], b.getX() + 26.0f, y, b.getWidth() - 32.0f, itemH - 2.0f, juce::Justification::left, true);
            }
        }

        void paintRackModules (juce::Graphics& g, const juce::Rectangle<float>& b)
        {
            const float unitH = b.getHeight() / 4.0f;

            struct UnitInfo {
                juce::String title;
                juce::String subtitle;
                juce::Colour accent;
            };

            const std::vector<UnitInfo> units = {
                { "TUBE SATURATOR", "VINTAGE ANALOG DRIVE", lookAndFeel.getPeachCoral() },
                { "COLOR RESONATOR", "ZAVALISHIN SVF SCALE BANK", lookAndFeel.getGoldenPeach() },
                { "ANALOG STEREO DELAY", "PING-PONG BOTANICAL ECHO", lookAndFeel.getSageMint() },
                { "LUSH BOTANICA REVERB", "DIFFUSE CLOUD RESONANCE", lookAndFeel.getBlushPink() }
            };

            for (size_t i = 0; i < units.size(); ++i)
            {
                auto unitBounds = juce::Rectangle<float> (b.getX() + 2.0f, b.getY() + i * unitH + 2.0f,
                                                          b.getWidth() - 4.0f, unitH - 4.0f);

                // Metallic/frosted rack faceplate matching Guitar Rig
                juce::ColourGradient faceGrad (juce::Colour::fromRGB (0x2D, 0x31, 0x35), unitBounds.getX(), unitBounds.getY(),
                                              juce::Colour::fromRGB (0x1F, 0x22, 0x24), unitBounds.getX(), unitBounds.getBottom(), false);
                g.setGradientFill (faceGrad);
                g.fillRoundedRectangle (unitBounds, 4.0f);

                g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.35f));
                g.drawRoundedRectangle (unitBounds, 4.0f, 1.0f);

                // Left color accent bar
                g.setColour (units[i].accent);
                g.fillRoundedRectangle (unitBounds.getX() + 3.0f, unitBounds.getY() + 4.0f, 5.0f, unitBounds.getHeight() - 8.0f, 2.0f);

                // Power toggle switch (yellow/amber circle like Guitar Rig)
                auto powerBounds = juce::Rectangle<float> (unitBounds.getX() + 16.0f, unitBounds.getCentreY() - 8.0f, 16.0f, 16.0f);
                g.setColour (lookAndFeel.getGoldenPeach());
                g.fillEllipse (powerBounds);
                g.setColour (lookAndFeel.getDarkEspresso());
                g.drawEllipse (powerBounds, 1.2f);
                g.setFont (juce::FontOptions (8.0f, juce::Font::bold));
                g.drawText ("I", powerBounds, juce::Justification::centred, true);

                // Rack Title
                g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
                g.setColour (lookAndFeel.getWhisperWhite());
                g.drawText (units[i].title, unitBounds.getX() + 40.0f, unitBounds.getY() + 10.0f, 160.0f, 16.0f, juce::Justification::left, true);

                g.setFont (juce::FontOptions (9.0f));
                g.setColour (lookAndFeel.getMutedSlate());
                g.drawText (units[i].subtitle, unitBounds.getX() + 40.0f, unitBounds.getY() + 28.0f, 160.0f, 14.0f, juce::Justification::left, true);
            }
        }

        void paintSignalFlow (juce::Graphics& g, const juce::Rectangle<float>& b)
        {
            // Title
            g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite());
            g.drawText ("SIGNAL FLOW", b.getX() + 10.0f, b.getY() + 8.0f, b.getWidth() - 20.0f, 18.0f, juce::Justification::left, true);

            // Flow tree blocks matching Guitar Rig right panel
            const float startY = b.getY() + 36.0f;
            const float blockW = 86.0f;
            const float blockH = 18.0f;
            const float cx = b.getCentreX();

            const juce::StringArray nodes = { "L/R IN", "TUBE COMP", "RESONATOR", "DELAY", "REVERB", "LIMITER", "OUT" };

            for (int i = 0; i < nodes.size(); ++i)
            {
                const float ny = startY + i * 26.0f;
                auto nodeRect = juce::Rectangle<float> (cx - blockW * 0.5f, ny, blockW, blockH);

                // Connecting vertical flow line
                if (i < nodes.size() - 1)
                {
                    g.setColour (lookAndFeel.getGoldenPeach().withAlpha (0.6f));
                    g.drawVerticalLine (static_cast<int>(cx), ny + blockH, ny + 26.0f);
                }

                g.setColour (i == 0 || i == nodes.size() - 1
                    ? lookAndFeel.getSageMint().withAlpha (0.3f)
                    : lookAndFeel.getPeachCoral().withAlpha (0.25f));
                g.fillRoundedRectangle (nodeRect, 3.0f);

                g.setColour (lookAndFeel.getGoldenPeach());
                g.drawRoundedRectangle (nodeRect, 3.0f, 1.0f);

                g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
                g.setColour (lookAndFeel.getWhisperWhite());
                g.drawText (nodes[i], nodeRect, juce::Justification::centred, true);
            }
        }
    };
}
