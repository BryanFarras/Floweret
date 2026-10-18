#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    class EnvelopeModule : public juce::Component
    {
    public:
        EnvelopeModule (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            titleLabel.setText ("ENV", juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            tabEnv1.setButtonText ("1");
            tabEnv2.setButtonText ("2");
            tabEnv3.setButtonText ("3");
            tabEnv1.setToggleState (true, juce::dontSendNotification);

            addAndMakeVisible (tabEnv1);
            addAndMakeVisible (tabEnv2);
            addAndMakeVisible (tabEnv3);

            setupDial (attackSlider, attackLabel, "ATTACK");
            setupDial (decaySlider, decayLabel, "DECAY");
            setupDial (sustainSlider, sustainLabel, "SUSTAIN");
            setupDial (releaseSlider, releaseLabel, "RELEASE");

            auto updateEnvCurve = [this]() { repaint(); };
            attackSlider.onValueChange  = updateEnvCurve;
            decaySlider.onValueChange   = updateEnvCurve;
            sustainSlider.onValueChange = updateEnvCurve;
            releaseSlider.onValueChange = updateEnvCurve;

            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::envAttack.getParamID(), attackSlider));
            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::envDecay.getParamID(), decaySlider));
            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::envSustain.getParamID(), sustainSlider));
            attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::envRelease.getParamID(), releaseSlider));
        }

        ~EnvelopeModule() override = default;

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

            // ADSR Graphical Curve Display Area
            const float dialAreaH = 72.0f;
            const float curveH = juce::jmax (40.0f, bounds.getHeight() - dialAreaH - 30.0f);
            auto curveBounds = juce::Rectangle<float> (bounds.getX() + 8.0f, 28.0f, bounds.getWidth() - 16.0f, curveH);
            g.setColour (juce::Colour::fromRGB (0x22, 0x26, 0x28));
            g.fillRoundedRectangle (curveBounds, 4.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.35f));
            g.drawRoundedRectangle (curveBounds, 4.0f, 1.0f);

            // Calculate ADSR curve points
            const float aNorm = juce::jlimit (0.05f, 0.35f, static_cast<float> (attackSlider.getValue()) / 2.0f);
            const float dNorm = juce::jlimit (0.05f, 0.35f, static_cast<float> (decaySlider.getValue()) / 2.0f);
            const float sNorm = juce::jlimit (0.0f, 1.0f, static_cast<float> (sustainSlider.getValue()));
            const float rNorm = juce::jlimit (0.05f, 0.35f, static_cast<float> (releaseSlider.getValue()) / 2.0f);

            const float w = curveBounds.getWidth() - 16.0f;
            const float h = curveBounds.getHeight() - 12.0f;
            const float x0 = curveBounds.getX() + 8.0f;
            const float y0 = curveBounds.getBottom() - 6.0f;
            const float yTop = curveBounds.getY() + 6.0f;

            const float xA = x0 + aNorm * (w * 0.3f);
            const float xD = xA + dNorm * (w * 0.3f);
            const float yS = y0 - sNorm * h;
            const float xS = xD + w * 0.2f;
            const float xR = juce::jmin (x0 + w, xS + rNorm * (w * 0.3f));

            juce::Path curve;
            juce::Path fill;

            curve.startNewSubPath (x0, y0);
            fill.startNewSubPath (x0, y0);

            curve.lineTo (xA, yTop);
            fill.lineTo (xA, yTop);

            curve.lineTo (xD, yS);
            fill.lineTo (xD, yS);

            curve.lineTo (xS, yS);
            fill.lineTo (xS, yS);

            curve.lineTo (xR, y0);
            fill.lineTo (xR, y0);

            fill.closeSubPath();

            // Underfill gradient
            juce::ColourGradient fillGrad (lookAndFeel.getPeachCoral().withAlpha (0.35f), x0, yTop,
                                          lookAndFeel.getGoldenPeach().withAlpha (0.04f), x0, y0, false);
            g.setGradientFill (fillGrad);
            g.fillPath (fill);

            // Stroke
            g.setColour (lookAndFeel.getGoldenPeach());
            g.strokePath (curve, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        void resized() override
        {
            titleLabel.setBounds (8, 4, 40, 18);
            tabEnv1.setBounds (54, 4, 22, 18);
            tabEnv2.setBounds (78, 4, 22, 18);
            tabEnv3.setBounds (102, 4, 22, 18);

            // 4 ADSR Dials anchored dynamically to bottom
            const int dialW = (getWidth() - 16) / 4;
            const int dialSize = juce::jlimit (32, 50, dialW - 4);
            const int dialY = getHeight() - dialSize - 10;
            const int labelY = dialY - 16;

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = 8 + idx * dialW;
                l.setBounds (dx, labelY, dialW, 14);
                s.setBounds (dx + (dialW - dialSize) / 2, dialY, dialSize, dialSize);
            };

            placeDial (attackSlider, attackLabel, 0);
            placeDial (decaySlider, decayLabel, 1);
            placeDial (sustainSlider, sustainLabel, 2);
            placeDial (releaseSlider, releaseLabel, 3);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::Label titleLabel;
        juce::TextButton tabEnv1, tabEnv2, tabEnv3;

        juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
        juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

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
