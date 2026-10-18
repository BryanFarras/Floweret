#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    //==============================================================================
    // Vital / Serum Style Compact Wavetable Display
    // Strictly follows "Soft Peachy Delight" palette (Golden Peach, Peach Coral, Apricot)
    //==============================================================================
    class VitalWaveDisplay : public juce::Component
    {
    public:
        VitalWaveDisplay (BotanicaLookAndFeel& laf)
            : lookAndFeel (laf)
        {
            points.resize (100, 0.0f);
            updateWaveform (0.0f);
        }

        void updateWaveform (float pos)
        {
            currentPos = juce::jlimit (0.0f, 1.0f, pos);

            // Generate morphed shape matching the geometric saw/pulse shown in Vital
            for (size_t i = 0; i < points.size(); ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (points.size() - 1);
                const float angle = t * juce::MathConstants<float>::twoPi;

                const float sinV = std::sin (angle);
                // Asymmetric saw / triangle shape
                const float sawV = t < 0.5f ? (1.0f - 4.0f * t) * 0.7f : (3.0f - 4.0f * t) * 0.7f;
                // Vital geometric formant wave
                const float formantV = (t < 0.5f ? -0.8f + 1.6f * t : 0.8f - 1.6f * (t - 0.5f));

                if (currentPos < 0.5f)
                {
                    const float blend = currentPos / 0.5f;
                    points[i] = sawV * (1.0f - blend) + sinV * blend;
                }
                else
                {
                    const float blend = (currentPos - 0.5f) / 0.5f;
                    points[i] = sinV * (1.0f - blend) + formantV * blend;
                }
            }
            repaint();
        }

        std::function<void(float)> onPositionChanged;

        void mouseDown (const juce::MouseEvent& e) override { handleScrub (e.x); }
        void mouseDrag (const juce::MouseEvent& e) override { handleScrub (e.x); }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // 1. Dark Screen Background (#1E2224)
            g.setColour (juce::Colour::fromRGB (0x1E, 0x22, 0x24));
            g.fillRoundedRectangle (bounds, 5.0f);

            // Platinum border
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.45f));
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            // 2. Top Header Bar: "Init" and ">" chevron
            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite());
            g.drawText ("Init", bounds.getX(), bounds.getY() + 3.0f, bounds.getWidth(), 14.0f, juce::Justification::centred, true);

            // Chevron ">" in top right
            juce::Path chevron;
            const float chX = bounds.getRight() - 14.0f;
            const float chY = bounds.getY() + 7.0f;
            chevron.startNewSubPath (chX, chY);
            chevron.lineTo (chX + 4.0f, chY + 3.0f);
            chevron.lineTo (chX, chY + 6.0f);
            g.setColour (lookAndFeel.getPeachCoral());
            g.strokePath (chevron, juce::PathStrokeType (1.5f));

            // 3. Center Waveform
            const float centerY = bounds.getY() + bounds.getHeight() * 0.52f;
            const float waveH = bounds.getHeight() * 0.34f;
            const float waveLeft = bounds.getX() + 6.0f;
            const float waveWidth = bounds.getWidth() - 12.0f;

            // Zero line
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.25f));
            g.drawHorizontalLine (static_cast<int>(centerY), waveLeft, waveLeft + waveWidth);

            juce::Path wavePath;
            juce::Path fillPath;

            for (size_t i = 0; i < points.size(); ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (points.size() - 1);
                const float x = waveLeft + t * waveWidth;
                const float y = centerY - points[i] * waveH;

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

            // Translucent gradient fill strictly adhering to "Soft Peachy Delight" palette
            juce::ColourGradient fillGrad (lookAndFeel.getGoldenPeach().withAlpha (0.65f), waveLeft, centerY - waveH,
                                          lookAndFeel.getPeachCoral().withAlpha (0.12f), waveLeft, centerY + waveH, false);
            g.setGradientFill (fillGrad);
            g.fillPath (fillPath);

            // Glow stroke & sharp wave stroke in Soft Peachy Delight colors (no purple/blue)
            g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.45f));
            g.strokePath (wavePath, juce::PathStrokeType (3.2f));

            g.setColour (lookAndFeel.getGoldenPeach());
            g.strokePath (wavePath, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // 4. Bottom Indicators: "2D" on left and Pencil icon on right
            g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getPastelApricot());
            g.drawText ("2D", bounds.getX() + 6.0f, bounds.getBottom() - 16.0f, 22.0f, 12.0f, juce::Justification::left, true);

            // Pencil Icon in bottom-right
            juce::Path pencil;
            const float px = bounds.getRight() - 16.0f;
            const float py = bounds.getBottom() - 15.0f;
            pencil.startNewSubPath (px + 1.0f, py + 8.0f);
            pencil.lineTo (px, py + 10.0f);
            pencil.lineTo (px + 2.0f, py + 10.0f);
            pencil.lineTo (px + 9.0f, py + 3.0f);
            pencil.lineTo (px + 7.0f, py + 1.0f);
            pencil.closeSubPath();
            g.setColour (lookAndFeel.getPastelApricot());
            g.fillPath (pencil);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        std::vector<float> points;
        float currentPos = 0.0f;

        void handleScrub (int mouseX)
        {
            const auto bounds = getLocalBounds().toFloat().reduced (6.0f);
            const float newPos = juce::jlimit (0.0f, 1.0f, (static_cast<float>(mouseX) - bounds.getX()) / bounds.getWidth());
            updateWaveform (newPos);
            if (onPositionChanged)
                onPositionChanged (newPos);
        }
    };

    //==============================================================================
    // Unison Stepper Component (Matches 2nd reference picture: integer with ▲ / ▼)
    //==============================================================================
    class UnisonStepper : public juce::Slider
    {
    public:
        UnisonStepper (BotanicaLookAndFeel& laf)
            : lookAndFeel (laf)
        {
            setSliderStyle (juce::Slider::LinearBar);
            setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            setRange (1.0, 16.0, 1.0);
            setValue (1.0, juce::dontSendNotification);
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (0.5f);

            // 1. Recessed dark box (#1C2422 / deep slate matching 2nd picture)
            g.setColour (juce::Colour::fromRGB (0x1C, 0x24, 0x22));
            g.fillRoundedRectangle (bounds, 3.5f);

            // Outer border
            g.setColour (lookAndFeel.getDarkEspresso().withAlpha (0.85f));
            g.drawRoundedRectangle (bounds, 3.5f, 1.2f);

            // Inner bevel
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.25f));
            g.drawRoundedRectangle (bounds.reduced (1.0f), 2.5f, 0.7f);

            // 2. Integer readout in bold font
            const int val = static_cast<int> (std::round (getValue()));
            const auto numArea = bounds.withTrimmedRight (14.0f);
            g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getGoldenPeach());
            g.drawText (juce::String (val), numArea, juce::Justification::centred, false);

            // 3. Vertically stacked ▲ (up) and ▼ (down) triangles
            const float rightColX = bounds.getRight() - 8.5f;
            const float midY = bounds.getCentreY();

            // Up triangle: ▲
            juce::Path upArrow;
            upArrow.startNewSubPath (rightColX - 3.2f, midY - 2.0f);
            upArrow.lineTo (rightColX + 3.2f, midY - 2.0f);
            upArrow.lineTo (rightColX, midY - 6.0f);
            upArrow.closeSubPath();

            // Down triangle: ▼
            juce::Path downArrow;
            downArrow.startNewSubPath (rightColX - 3.2f, midY + 2.0f);
            downArrow.lineTo (rightColX + 3.2f, midY + 2.0f);
            downArrow.lineTo (rightColX, midY + 6.0f);
            downArrow.closeSubPath();

            // Fill arrows with hover highlight
            if (hoveredPart == HoverPart::Up)
                g.setColour (lookAndFeel.getPeachCoral());
            else
                g.setColour (lookAndFeel.getSageMint().withAlpha (0.85f));
            g.fillPath (upArrow);

            if (hoveredPart == HoverPart::Down)
                g.setColour (lookAndFeel.getPeachCoral());
            else
                g.setColour (lookAndFeel.getSageMint().withAlpha (0.85f));
            g.fillPath (downArrow);
        }

        void mouseMove (const juce::MouseEvent& e) override
        {
            updateHover (e.getPosition());
        }

        void mouseExit (const juce::MouseEvent& /*e*/) override
        {
            hoveredPart = HoverPart::None;
            repaint();
        }

        void mouseDown (const juce::MouseEvent& e) override
        {
            lastMousePos = e.getPosition();
            initialValue = getValue();

            const float arrowDividerX = getWidth() - 14.0f;
            if (e.x >= arrowDividerX)
            {
                if (e.y < getHeight() / 2)
                    setValue (juce::jlimit (1.0, 16.0, getValue() + 1.0), juce::sendNotificationAsync);
                else
                    setValue (juce::jlimit (1.0, 16.0, getValue() - 1.0), juce::sendNotificationAsync);
            }
            else
            {
                isDragging = true;
            }
        }

        void mouseDrag (const juce::MouseEvent& e) override
        {
            if (isDragging)
            {
                const int deltaY = lastMousePos.y - e.y;
                const double step = deltaY / 8.0;
                setValue (juce::jlimit (1.0, 16.0, std::round (initialValue + step)), juce::sendNotificationAsync);
            }
        }

        void mouseUp (const juce::MouseEvent& /*e*/) override
        {
            isDragging = false;
        }

        void mouseWheelMove (const juce::MouseEvent& /*e*/, const juce::MouseWheelDetails& wheel) override
        {
            if (wheel.deltaY > 0.0f)
                setValue (juce::jlimit (1.0, 16.0, getValue() + 1.0), juce::sendNotificationAsync);
            else if (wheel.deltaY < 0.0f)
                setValue (juce::jlimit (1.0, 16.0, getValue() - 1.0), juce::sendNotificationAsync);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        enum class HoverPart { None, Up, Down, Body };
        HoverPart hoveredPart = HoverPart::None;
        juce::Point<int> lastMousePos;
        double initialValue = 1.0;
        bool isDragging = false;

        void updateHover (juce::Point<int> pos)
        {
            const float arrowDividerX = getWidth() - 14.0f;
            HoverPart newHover = HoverPart::Body;

            if (pos.x >= arrowDividerX)
            {
                if (pos.y < getHeight() / 2)
                    newHover = HoverPart::Up;
                else
                    newHover = HoverPart::Down;
            }

            if (newHover != hoveredPart)
            {
                hoveredPart = newHover;
                repaint();
            }
        }
    };

    //==============================================================================
    // Oscillator Parameter Group Helper
    //==============================================================================
    enum class OscId { A = 0, B, C };

    struct OscParamGroup
    {
        juce::ParameterID active;
        juce::ParameterID position;
        juce::ParameterID warpMode;
        juce::ParameterID warpAmount;
        juce::ParameterID tune;
        juce::ParameterID pan;
        juce::ParameterID level;
        juce::ParameterID unison;
        juce::ParameterID detuneFine;
        juce::ParameterID mix;
    };

    inline OscParamGroup getOscParams (OscId id)
    {
        switch (id)
        {
            case OscId::A:
                return {
                    ParamIDs::wtActive, ParamIDs::wtPosition, ParamIDs::wtWarpMode,
                    ParamIDs::wtWarpAmount, ParamIDs::wtDetune, ParamIDs::wtPan,
                    ParamIDs::wtLevel, ParamIDs::wtUnison, ParamIDs::wtDetuneFine, ParamIDs::wtMix
                };
            case OscId::B:
                return {
                    ParamIDs::wtBActive, ParamIDs::wtBPosition, ParamIDs::wtBWarpMode,
                    ParamIDs::wtBWarpAmount, ParamIDs::wtBDetune, ParamIDs::wtBPan,
                    ParamIDs::wtBLevel, ParamIDs::wtBUnison, ParamIDs::wtBDetuneFine, ParamIDs::wtBMix
                };
            case OscId::C:
            default:
                return {
                    ParamIDs::wtCActive, ParamIDs::wtCPosition, ParamIDs::wtCWarpMode,
                    ParamIDs::wtCWarpAmount, ParamIDs::wtCTune, ParamIDs::wtCPan,
                    ParamIDs::wtCLevel, ParamIDs::wtCUnison, ParamIDs::wtCDetuneFine, ParamIDs::wtCMix
                };
        }
    }

    //==============================================================================
    // OscillatorModule with layout and independent parameter binding
    //==============================================================================
    class OscillatorModule : public juce::Component
    {
    public:
        enum class OscMode { Wavetable = 0, Spectral };

        OscillatorModule (OscId oscId, BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : oscIdentifier (oscId),
              name (oscId == OscId::A ? "OSC A" : (oscId == OscId::B ? "OSC B" : "OSC C")),
              lookAndFeel (laf),
              waveDisplay (laf),
              unisonStepper (laf)
        {
            initModule (apvts);
        }

        OscillatorModule (const juce::String& oscName, BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : oscIdentifier (oscName.containsIgnoreCase ("B") ? OscId::B : (oscName.containsIgnoreCase ("C") ? OscId::C : OscId::A)),
              name (oscName),
              lookAndFeel (laf),
              waveDisplay (laf),
              unisonStepper (laf)
        {
            initModule (apvts);
        }

        ~OscillatorModule() override = default;

        void setMode (OscMode newMode)
        {
            currentMode = newMode;
            wtModeBtn.setToggleState (currentMode == OscMode::Wavetable, juce::dontSendNotification);
            specModeBtn.setToggleState (currentMode == OscMode::Spectral, juce::dontSendNotification);

            if (currentMode == OscMode::Wavetable)
            {
                posLabel.setText ("POS", juce::dontSendNotification);
                warpLabel.setText ("WARP", juce::dontSendNotification);
            }
            else
            {
                posLabel.setText ("TILT", juce::dontSendNotification);
                warpLabel.setText ("SMEAR", juce::dontSendNotification);
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
        }

        void resized() override
        {
            const auto bounds = getLocalBounds().reduced (3);

            // 1. Left Section: Wave Display & Horizontal POS Slider
            const int waveW = 168;
            const int waveH = bounds.getHeight() - 26;

            waveDisplay.setBounds (bounds.getX() + 2, bounds.getY() + 2, waveW, waveH);

            const int sliderY = bounds.getY() + waveH + 5;
            horizPosSlider.setBounds (bounds.getX() + 2, sliderY, waveW - 32, 16);
            horizPosLabel.setBounds  (bounds.getX() + waveW - 28, sliderY - 1, 30, 16);

            // 2. Right Section Header (Power, Title, WT/SPEC, Warp dropdown)
            const int rightX = bounds.getX() + waveW + 10;
            const int headerY = bounds.getY() + 2;

            powerBtn.setBounds    (rightX, headerY + 1, 18, 18);
            titleLabel.setBounds  (rightX + 22, headerY, 46, 20);

            wtModeBtn.setBounds   (rightX + 70, headerY + 1, 26, 18);
            specModeBtn.setBounds (rightX + 98, headerY + 1, 38, 18);

            const int warpW = 118;
            warpModeBox.setBounds (rightX + 140, headerY + 1, warpW, 19);

            // 3. Knobs Section
            const int knobsY = headerY + 24;

            // Row of 5 Main Dials: POS, WARP, TUNE, PAN, LEVEL
            const int dialW = 36;
            const int dialSize = 34;

            auto placeDial = [&](juce::Slider& s, juce::Label& l, int idx) {
                const int dx = rightX + idx * (dialW + 2);
                l.setBounds (dx, knobsY + 2, dialW, 13);
                s.setBounds (dx + (dialW - dialSize) / 2, knobsY + 16, dialSize, dialSize);
            };

            placeDial (posSlider, posLabel, 0);
            placeDial (warpSlider, warpLabel, 1);
            placeDial (detuneSlider, detuneLabel, 2);
            placeDial (panSlider, panLabel, 3);
            placeDial (levelSlider, levelLabel, 4);

            // 4. UNISON (Incrementing Stepper) & DETUNE Column
            const int unisonX = rightX + 5 * (dialW + 2) + 6;
            const int colW = 38;
            const int smallDialSize = 24;

            unisonLabel.setBounds   (unisonX, knobsY + 2, colW, 12);
            unisonStepper.setBounds (unisonX, knobsY + 16, colW, 20);

            detuneFineLabel.setBounds  (unisonX, knobsY + 38, colW, 12);
            detuneFineSlider.setBounds (unisonX + (colW - smallDialSize) / 2, knobsY + 50, smallDialSize, smallDialSize);

            // 5. MIX Column
            const int mixX = unisonX + colW + 6;
            const int mixW = 44;
            const int mixDialSize = 38;

            mixLabel.setBounds  (mixX, knobsY + 2, mixW, 13);
            mixSlider.setBounds (mixX + (mixW - mixDialSize) / 2, knobsY + 16, mixDialSize, mixDialSize);
        }

    private:
        OscId oscIdentifier;
        juce::String name;
        BotanicaLookAndFeel& lookAndFeel;
        OscMode currentMode = OscMode::Wavetable;

        // Left section
        VitalWaveDisplay waveDisplay;
        juce::Slider horizPosSlider;
        juce::Label horizPosLabel;

        // Right header
        juce::TextButton powerBtn;
        juce::Label titleLabel;
        juce::TextButton wtModeBtn;
        juce::TextButton specModeBtn;
        juce::ComboBox warpModeBox;

        // Knobs
        juce::Slider posSlider, warpSlider, detuneSlider, panSlider, levelSlider;
        juce::Label posLabel, warpLabel, detuneLabel, panLabel, levelLabel;

        UnisonStepper unisonStepper;
        juce::Slider detuneFineSlider;
        juce::Label unisonLabel, detuneFineLabel;

        juce::Slider mixSlider;
        juce::Label mixLabel;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;

        void initModule (juce::AudioProcessorValueTreeState& apvts)
        {
            // 1. Left Section: Waveform display
            addAndMakeVisible (waveDisplay);

            // Horizontal POS slider below waveform
            horizPosSlider.setSliderStyle (juce::Slider::LinearHorizontal);
            horizPosSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            horizPosSlider.setLookAndFeel (&lookAndFeel);
            horizPosSlider.setRange (0.0, 1.0, 0.001);
            addAndMakeVisible (horizPosSlider);

            horizPosLabel.setText ("POS", juce::dontSendNotification);
            horizPosLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            horizPosLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (horizPosLabel);

            horizPosSlider.onValueChange = [this]() {
                const float val = static_cast<float> (horizPosSlider.getValue());
                waveDisplay.updateWaveform (val);
                if (posSlider.getValue() != horizPosSlider.getValue())
                    posSlider.setValue (val, juce::dontSendNotification);
            };

            waveDisplay.onPositionChanged = [this](float p) {
                horizPosSlider.setValue (p, juce::sendNotificationSync);
            };

            // 2. Right Section Header
            powerBtn.setButtonText ("I");
            powerBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (powerBtn);

            titleLabel.setText (name, juce::dontSendNotification);
            titleLabel.setFont (juce::FontOptions (11.5f, juce::Font::bold));
            titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (titleLabel);

            wtModeBtn.setButtonText ("WT");
            specModeBtn.setButtonText ("SPEC");
            wtModeBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (wtModeBtn);
            addAndMakeVisible (specModeBtn);

            wtModeBtn.onClick   = [this]() { setMode (OscMode::Wavetable); };
            specModeBtn.onClick = [this]() { setMode (OscMode::Spectral); };

            warpModeBox.addItemList (getWarpModeNames(), 1);
            warpModeBox.setSelectedId (1, juce::dontSendNotification);
            addAndMakeVisible (warpModeBox);

            // 3. Knobs: Row of 5 (POS, WARP, TUNE, PAN, LEVEL)
            setupDial (posSlider, posLabel, "POS");
            setupDial (warpSlider, warpLabel, "WARP");
            setupDial (detuneSlider, detuneLabel, "TUNE");
            setupDial (panSlider, panLabel, "PAN");
            setupDial (levelSlider, levelLabel, "LEVEL");

            posSlider.onValueChange = [this]() {
                if (horizPosSlider.getValue() != posSlider.getValue())
                    horizPosSlider.setValue (posSlider.getValue(), juce::sendNotificationSync);
            };

            // 4. Unison Stepper & Detune Column
            unisonLabel.setText ("UNISON", juce::dontSendNotification);
            unisonLabel.setFont (juce::FontOptions (8.5f, juce::Font::bold));
            unisonLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            unisonLabel.setJustificationType (juce::Justification::centred);
            addAndMakeVisible (unisonLabel);

            addAndMakeVisible (unisonStepper);

            setupDial (detuneFineSlider, detuneFineLabel, "DETUNE");
            detuneFineSlider.setRange (0.0, 1.0, 0.01);
            detuneFineSlider.setValue (0.15);

            // 5. Mix Column
            setupDial (mixSlider, mixLabel, "MIX");
            mixSlider.setRange (0.0, 1.0, 0.01);
            mixSlider.setValue (0.85);

            // APVTS Attachments to independent oscillator parameter group
            const auto params = getOscParams (oscIdentifier);

            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.position.getParamID(), posSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.warpAmount.getParamID(), warpSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.tune.getParamID(), detuneSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.pan.getParamID(), panSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.level.getParamID(), levelSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.unison.getParamID(), unisonStepper));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.detuneFine.getParamID(), detuneFineSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, params.mix.getParamID(), mixSlider));

            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, params.warpMode.getParamID(), warpModeBox));
            buttonAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                apvts, params.active.getParamID(), powerBtn));
        }

        void setupDial (juce::Slider& s, juce::Label& l, const juce::String& text)
        {
            l.setText (text, juce::dontSendNotification);
            l.setFont (juce::FontOptions (8.5f, juce::Font::bold));
            l.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            l.setJustificationType (juce::Justification::centred);
            addAndMakeVisible (l);

            s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            s.setLookAndFeel (&lookAndFeel);
            addAndMakeVisible (s);
        }
    };
}
