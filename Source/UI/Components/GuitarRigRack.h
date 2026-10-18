#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    //==============================================================================
    // Effect Types for Guitar Rig Modular Rack
    //==============================================================================
    enum class EffectType
    {
        TubeSaturator = 0,
        ColorResonator,
        AnalogDelay,
        BotanicaReverb,
        FormantShifter,
        VelvetNoise,
        MultibandSplitter,
        StereoLimiter
    };

    class GuitarRigRack : public juce::Component
    {
    public:
        std::function<void(const std::vector<int>&)> onOrderChanged;

        GuitarRigRack (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            // Initial rack setup: 4 core modules
            activeRack = {
                EffectType::TubeSaturator,
                EffectType::ColorResonator,
                EffectType::AnalogDelay,
                EffectType::BotanicaReverb
            };
            activeBypassed = { false, false, false, false };

            // Tube Compressor / Saturator Knobs
            setupKnob (distDriveKnob, "DRIVE", apvts, ParamIDs::distDrive.getParamID());
            setupKnob (distMixKnob, "MIX", apvts, ParamIDs::distMix.getParamID());

            // Color Resonator Knobs
            setupKnob (colorAmountKnob, "AMOUNT", apvts, ParamIDs::colorAmount.getParamID());
            setupKnob (colorResKnob, "RESONANCE", apvts, ParamIDs::colorResonance.getParamID());
            setupKnob (colorMorphKnob, "MORPH", apvts, ParamIDs::spectralMorph.getParamID());
            setupKnob (colorGateKnob, "GATE", apvts, ParamIDs::spectralGate.getParamID());

            // Delay Knobs
            setupKnob (delayTimeKnob, "TIME", apvts, ParamIDs::delayTime.getParamID());
            setupKnob (delayFbKnob, "FEEDBACK", apvts, ParamIDs::delayFeedback.getParamID());
            setupKnob (delayMixKnob, "MIX", apvts, ParamIDs::delayMix.getParamID());

            // Reverb Knobs
            setupKnob (reverbSizeKnob, "SIZE", apvts, ParamIDs::reverbSize.getParamID());
            setupKnob (reverbDampKnob, "DAMP", apvts, ParamIDs::reverbDamp.getParamID());
            setupKnob (bloomKnob, "BLOOM", apvts, ParamIDs::bloomAmount.getParamID());
            setupKnob (reverbMixKnob, "MIX", apvts, ParamIDs::reverbMix.getParamID());
        }

        ~GuitarRigRack() override = default;

        const std::vector<EffectType>& getActiveRack() const noexcept { return activeRack; }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);

            // Background canvas (#23272A)
            g.setColour (juce::Colour::fromRGB (0x23, 0x27, 0x2A));
            g.fillRoundedRectangle (bounds, 6.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.45f));
            g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

            const auto col1Bounds = getSidebarBounds();
            const auto col2Bounds = getRackBounds();
            const auto col3Bounds = getSignalFlowBounds();

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

            // Paint Floating Ghost Card if currently dragging
            if (isDragging)
            {
                paintFloatingGhost (g);
            }
        }

        void resized() override
        {
            const auto col2Bounds = getRackBounds();
            const int n = static_cast<int> (activeRack.size());
            if (n <= 0)
                return;

            const float unitH = col2Bounds.getHeight() / static_cast<float> (n);
            const int col2X = static_cast<int> (col2Bounds.getX());

            // Hide all knobs first
            distDriveKnob.setVisible (false);
            distMixKnob.setVisible (false);
            colorAmountKnob.setVisible (false);
            colorResKnob.setVisible (false);
            colorMorphKnob.setVisible (false);
            colorGateKnob.setVisible (false);
            delayTimeKnob.setVisible (false);
            delayFbKnob.setVisible (false);
            delayMixKnob.setVisible (false);
            reverbSizeKnob.setVisible (false);
            reverbDampKnob.setVisible (false);
            bloomKnob.setVisible (false);
            reverbMixKnob.setVisible (false);

            for (int i = 0; i < n; ++i)
            {
                const int uY = static_cast<int> (col2Bounds.getY() + i * unitH);
                const int uH = static_cast<int> (unitH);
                const int knobY = uY + (uH - 52) / 2 + 1;

                switch (activeRack[i])
                {
                    case EffectType::TubeSaturator:
                        distDriveKnob.setBounds (col2X + 220, knobY, 50, 52);
                        distMixKnob.setBounds   (col2X + 290, knobY, 50, 52);
                        distDriveKnob.setVisible (true);
                        distMixKnob.setVisible (true);
                        break;

                    case EffectType::ColorResonator:
                        colorAmountKnob.setBounds (col2X + 180, knobY, 50, 52);
                        colorResKnob.setBounds    (col2X + 245, knobY, 50, 52);
                        colorMorphKnob.setBounds  (col2X + 310, knobY, 50, 52);
                        colorGateKnob.setBounds   (col2X + 375, knobY, 50, 52);
                        colorAmountKnob.setVisible (true);
                        colorResKnob.setVisible (true);
                        colorMorphKnob.setVisible (true);
                        colorGateKnob.setVisible (true);
                        break;

                    case EffectType::AnalogDelay:
                        delayTimeKnob.setBounds (col2X + 200, knobY, 50, 52);
                        delayFbKnob.setBounds   (col2X + 270, knobY, 50, 52);
                        delayMixKnob.setBounds  (col2X + 340, knobY, 50, 52);
                        delayTimeKnob.setVisible (true);
                        delayFbKnob.setVisible (true);
                        delayMixKnob.setVisible (true);
                        break;

                    case EffectType::BotanicaReverb:
                        reverbSizeKnob.setBounds (col2X + 170, knobY, 50, 52);
                        reverbDampKnob.setBounds (col2X + 235, knobY, 50, 52);
                        bloomKnob.setBounds      (col2X + 300, knobY, 50, 52);
                        reverbMixKnob.setBounds  (col2X + 365, knobY, 50, 52);
                        reverbSizeKnob.setVisible (true);
                        reverbDampKnob.setVisible (true);
                        bloomKnob.setVisible (true);
                        reverbMixKnob.setVisible (true);
                        break;

                    default:
                        break;
                }
            }
        }

        //==============================================================================
        // Mouse Drag and Drop Overrides
        //==============================================================================
        void mouseDown (const juce::MouseEvent& e) override
        {
            const auto col1Bounds = getSidebarBounds();
            const auto col2Bounds = getRackBounds();

            if (col2Bounds.contains (e.position))
            {
                const int slot = getSlotIndexAtY (e.y);
                if (slot >= 0 && slot < static_cast<int> (activeRack.size()))
                {
                    auto unitBounds = getSlotBounds (slot);

                    // 1. Power toggle switch clicked
                    auto powerBounds = juce::Rectangle<float> (unitBounds.getX() + 14.0f, unitBounds.getCentreY() - 9.0f, 18.0f, 18.0f);
                    if (powerBounds.contains (e.position))
                    {
                        activeBypassed[slot] = !activeBypassed[slot];
                        notifyOrderChanged();
                        repaint();
                        return;
                    }

                    // 2. Remove button [×] clicked
                    auto removeBounds = juce::Rectangle<float> (unitBounds.getRight() - 24.0f, unitBounds.getY() + 8.0f, 16.0f, 16.0f);
                    if (removeBounds.contains (e.position) && activeRack.size() > 1)
                    {
                        activeRack.erase (activeRack.begin() + slot);
                        activeBypassed.erase (activeBypassed.begin() + slot);
                        notifyOrderChanged();
                        resized();
                        repaint();
                        return;
                    }

                    // 3. Initiate Drag from Rack
                    isDragging = true;
                    dragSource = DragSource::Rack;
                    draggedSlot = slot;
                    draggedEffect = activeRack[slot];
                    dragPos = e.position;
                    dropInsertIndex = slot;
                    setMouseCursor (juce::MouseCursor::DraggingHandCursor);
                    repaint();
                }
            }
            else if (col1Bounds.contains (e.position))
            {
                const int itemIdx = getSidebarItemAtY (e.y);
                if (itemIdx >= 0 && itemIdx < static_cast<int> (allEffects.size()))
                {
                    isDragging = true;
                    dragSource = DragSource::Sidebar;
                    draggedSlot = -1;
                    draggedEffect = allEffects[itemIdx];
                    dragPos = e.position;
                    dropInsertIndex = static_cast<int> (activeRack.size());
                    setMouseCursor (juce::MouseCursor::DraggingHandCursor);
                    repaint();
                }
            }
        }

        void mouseDrag (const juce::MouseEvent& e) override
        {
            if (isDragging)
            {
                dragPos = e.position;

                const auto col2Bounds = getRackBounds();
                if (col2Bounds.contains (e.position))
                {
                    const float unitH = col2Bounds.getHeight() / static_cast<float> (std::max (1, (int) activeRack.size()));
                    const float relativeY = e.y - col2Bounds.getY();
                    dropInsertIndex = juce::jlimit (0, (int) activeRack.size(), static_cast<int> (std::round (relativeY / unitH)));
                }
                else
                {
                    dropInsertIndex = -1;
                }

                repaint();
            }
        }

        void mouseUp (const juce::MouseEvent& e) override
        {
            if (isDragging)
            {
                const auto col2Bounds = getRackBounds();
                if (dropInsertIndex >= 0 && col2Bounds.contains (e.position))
                {
                    if (dragSource == DragSource::Rack)
                    {
                        if (draggedSlot >= 0 && draggedSlot < static_cast<int> (activeRack.size()))
                        {
                            const auto eff = activeRack[draggedSlot];
                            const bool b = activeBypassed[draggedSlot];

                            activeRack.erase (activeRack.begin() + draggedSlot);
                            activeBypassed.erase (activeBypassed.begin() + draggedSlot);

                            int insertIdx = dropInsertIndex;
                            if (insertIdx > draggedSlot)
                                insertIdx--;
                            insertIdx = juce::jlimit (0, (int) activeRack.size(), insertIdx);

                            activeRack.insert (activeRack.begin() + insertIdx, eff);
                            activeBypassed.insert (activeBypassed.begin() + insertIdx, b);
                        }
                    }
                    else if (dragSource == DragSource::Sidebar)
                    {
                        auto it = std::find (activeRack.begin(), activeRack.end(), draggedEffect);
                        if (it != activeRack.end())
                        {
                            int oldIdx = static_cast<int> (std::distance (activeRack.begin(), it));
                            bool b = activeBypassed[oldIdx];
                            activeRack.erase (it);
                            activeBypassed.erase (activeBypassed.begin() + oldIdx);

                            int insertIdx = dropInsertIndex;
                            if (insertIdx > oldIdx)
                                insertIdx--;
                            insertIdx = juce::jlimit (0, (int) activeRack.size(), insertIdx);

                            activeRack.insert (activeRack.begin() + insertIdx, draggedEffect);
                            activeBypassed.insert (activeBypassed.begin() + insertIdx, b);
                        }
                        else
                        {
                            int insertIdx = juce::jlimit (0, (int) activeRack.size(), dropInsertIndex);
                            activeRack.insert (activeRack.begin() + insertIdx, draggedEffect);
                            activeBypassed.insert (activeBypassed.begin() + insertIdx, false);
                        }
                    }

                    notifyOrderChanged();
                    resized();
                }
                else if (dragSource == DragSource::Rack && !col2Bounds.contains (e.position) && activeRack.size() > 1)
                {
                    // Dragged out of the rack -> remove it
                    if (draggedSlot >= 0 && draggedSlot < static_cast<int> (activeRack.size()))
                    {
                        activeRack.erase (activeRack.begin() + draggedSlot);
                        activeBypassed.erase (activeBypassed.begin() + draggedSlot);
                        notifyOrderChanged();
                        resized();
                    }
                }

                isDragging = false;
                dropInsertIndex = -1;
                draggedSlot = -1;
                setMouseCursor (juce::MouseCursor::NormalCursor);
                repaint();
            }
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
        LabeledKnob colorAmountKnob, colorResKnob, colorMorphKnob, colorGateKnob;
        LabeledKnob delayTimeKnob, delayFbKnob, delayMixKnob;
        LabeledKnob reverbSizeKnob, reverbDampKnob, bloomKnob, reverbMixKnob;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

        // Dynamic rack state
        std::vector<EffectType> activeRack;
        std::vector<bool> activeBypassed;

        const std::vector<EffectType> allEffects = {
            EffectType::TubeSaturator,
            EffectType::ColorResonator,
            EffectType::AnalogDelay,
            EffectType::BotanicaReverb,
            EffectType::FormantShifter,
            EffectType::VelvetNoise,
            EffectType::MultibandSplitter,
            EffectType::StereoLimiter
        };

        // Drag state
        enum class DragSource { None, Rack, Sidebar };
        bool isDragging = false;
        DragSource dragSource = DragSource::None;
        int draggedSlot = -1;
        EffectType draggedEffect = EffectType::TubeSaturator;
        juce::Point<float> dragPos;
        int dropInsertIndex = -1;

        struct UnitMetadata
        {
            juce::String title;
            juce::String subtitle;
            juce::Colour accent;
            juce::String flowNodeName;
        };

        UnitMetadata getMetadata (EffectType type) const
        {
            switch (type)
            {
                case EffectType::TubeSaturator:
                    return { "TUBE SATURATOR", "VINTAGE ANALOG DRIVE", lookAndFeel.getPeachCoral(), "TUBE COMP" };
                case EffectType::ColorResonator:
                    return { "COLOR RESONATOR", "ZAVALISHIN SVF SCALE BANK", lookAndFeel.getGoldenPeach(), "RESONATOR" };
                case EffectType::AnalogDelay:
                    return { "ANALOG STEREO DELAY", "PING-PONG BOTANICAL ECHO", lookAndFeel.getSageMint(), "DELAY" };
                case EffectType::BotanicaReverb:
                    return { "LUSH BOTANICA REVERB", "DIFFUSE CLOUD RESONANCE", lookAndFeel.getBlushPink(), "REVERB" };
                case EffectType::FormantShifter:
                    return { "FORMANT SHIFTER", "SPECTRAL TIMBRE MORPH", lookAndFeel.getPastelApricot(), "FORMANT" };
                case EffectType::VelvetNoise:
                    return { "VELVET NOISE EXCITER", "SPARSE IMPULSE TEXTURE", lookAndFeel.getCreamyApricot(), "VELVET" };
                case EffectType::MultibandSplitter:
                    return { "MULTIBAND SPLITTER", "3-BAND LINKWITZ-RILEY", lookAndFeel.getPlatinumBorder(), "SPLIT" };
                case EffectType::StereoLimiter:
                    return { "STEREO PEAK LIMITER", "TRANSPARENT TRUE PEAK", lookAndFeel.getPeachLight(), "LIMITER" };
                default:
                    return { "EFFECT", "DSP MODULE", lookAndFeel.getGoldenPeach(), "FX" };
            }
        }

        void notifyOrderChanged()
        {
            if (onOrderChanged)
            {
                std::vector<int> chain;
                for (size_t i = 0; i < activeRack.size(); ++i)
                {
                    if (!activeBypassed[i])
                        chain.push_back (static_cast<int> (activeRack[i]));
                }
                onOrderChanged (chain);
            }
        }

        juce::Rectangle<float> getSidebarBounds() const
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            return juce::Rectangle<float> (bounds.getX(), bounds.getY(), 160.0f, bounds.getHeight());
        }

        juce::Rectangle<float> getRackBounds() const
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            const float col1W = 160.0f;
            const float col3W = 170.0f;
            const float col2W = bounds.getWidth() - col1W - col3W - 8.0f;
            return juce::Rectangle<float> (bounds.getX() + col1W + 4.0f, bounds.getY(), col2W, bounds.getHeight());
        }

        juce::Rectangle<float> getSignalFlowBounds() const
        {
            const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
            const float col3W = 170.0f;
            return juce::Rectangle<float> (bounds.getRight() - col3W, bounds.getY(), col3W, bounds.getHeight());
        }

        int getSlotIndexAtY (float y) const
        {
            const auto b = getRackBounds();
            if (activeRack.empty())
                return -1;
            const float unitH = b.getHeight() / static_cast<float> (activeRack.size());
            const int idx = static_cast<int> ((y - b.getY()) / unitH);
            return juce::jlimit (0, static_cast<int> (activeRack.size()) - 1, idx);
        }

        juce::Rectangle<float> getSlotBounds (int slot) const
        {
            const auto b = getRackBounds();
            const float unitH = b.getHeight() / static_cast<float> (std::max (1, (int) activeRack.size()));
            return juce::Rectangle<float> (b.getX() + 2.0f, b.getY() + slot * unitH + 2.0f, b.getWidth() - 4.0f, unitH - 4.0f);
        }

        int getSidebarItemAtY (float y) const
        {
            const float itemY = getSidebarBounds().getY() + 60.0f;
            const float itemH = 22.0f;
            const int idx = static_cast<int> ((y - itemY) / itemH);
            if (idx >= 0 && idx < static_cast<int> (allEffects.size()))
                return idx;
            return -1;
        }

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

            // Search box
            auto searchBox = juce::Rectangle<float> (b.getX() + 8.0f, b.getY() + 32.0f, b.getWidth() - 16.0f, 20.0f);
            g.setColour (juce::Colour::fromRGB (0x1B, 0x1E, 0x20));
            g.fillRoundedRectangle (searchBox, 3.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.3f));
            g.drawRoundedRectangle (searchBox, 3.0f, 1.0f);

            g.setFont (juce::FontOptions (10.0f));
            g.setColour (lookAndFeel.getMutedSlate());
            g.drawText ("Search Components...", searchBox.reduced (6.0f, 0.0f), juce::Justification::centredLeft, true);

            // Component list items
            const float itemY = b.getY() + 60.0f;
            const float itemH = 22.0f;

            for (size_t i = 0; i < allEffects.size(); ++i)
            {
                const float y = itemY + static_cast<float> (i) * itemH;
                const auto meta = getMetadata (allEffects[i]);
                const bool isActive = std::find (activeRack.begin(), activeRack.end(), allEffects[i]) != activeRack.end();

                if (isActive)
                {
                    g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.15f));
                    g.fillRoundedRectangle (b.getX() + 6.0f, y, b.getWidth() - 12.0f, itemH - 2.0f, 3.0f);

                    // Active glowing dot
                    g.setColour (meta.accent);
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
                g.drawText (meta.title, b.getX() + 26.0f, y, b.getWidth() - 32.0f, itemH - 2.0f, juce::Justification::left, true);
            }
        }

        void paintRackModules (juce::Graphics& g, const juce::Rectangle<float>& b)
        {
            const int n = static_cast<int> (activeRack.size());
            if (n <= 0)
                return;

            const float unitH = b.getHeight() / static_cast<float> (n);

            for (size_t i = 0; i < activeRack.size(); ++i)
            {
                auto unitBounds = getSlotBounds (static_cast<int> (i));
                const auto meta = getMetadata (activeRack[i]);
                const bool isBypassed = activeBypassed[i];

                // If currently being dragged from rack, draw dashed placeholder slot
                if (isDragging && dragSource == DragSource::Rack && static_cast<int> (i) == draggedSlot)
                {
                    g.setColour (juce::Colour::fromRGB (0x18, 0x1B, 0x1D));
                    g.fillRoundedRectangle (unitBounds, 4.0f);
                    g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.25f));
                    g.drawRoundedRectangle (unitBounds, 4.0f, 1.0f);
                    continue;
                }

                // Metallic/frosted rack faceplate matching Guitar Rig
                juce::ColourGradient faceGrad (juce::Colour::fromRGB (0x2D, 0x31, 0x35), unitBounds.getX(), unitBounds.getY(),
                                              juce::Colour::fromRGB (0x1F, 0x22, 0x24), unitBounds.getX(), unitBounds.getBottom(), false);
                g.setGradientFill (faceGrad);
                g.fillRoundedRectangle (unitBounds, 4.0f);

                g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (isBypassed ? 0.15f : 0.35f));
                g.drawRoundedRectangle (unitBounds, 4.0f, 1.0f);

                // Left color accent bar
                g.setColour (isBypassed ? meta.accent.withAlpha (0.35f) : meta.accent);
                g.fillRoundedRectangle (unitBounds.getX() + 3.0f, unitBounds.getY() + 4.0f, 5.0f, unitBounds.getHeight() - 8.0f, 2.0f);

                // Power toggle switch [I] (Amber/Golden Peach when active)
                auto powerBounds = juce::Rectangle<float> (unitBounds.getX() + 15.0f, unitBounds.getCentreY() - 9.0f, 18.0f, 18.0f);
                if (isBypassed)
                {
                    g.setColour (juce::Colour::fromRGB (0x2A, 0x2E, 0x30));
                    g.fillEllipse (powerBounds);
                    g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.6f));
                    g.drawEllipse (powerBounds, 1.0f);
                    g.setFont (juce::FontOptions (8.0f, juce::Font::bold));
                    g.drawText ("I", powerBounds, juce::Justification::centred, true);
                }
                else
                {
                    g.setColour (lookAndFeel.getGoldenPeach());
                    g.fillEllipse (powerBounds);
                    g.setColour (lookAndFeel.getDarkEspresso());
                    g.drawEllipse (powerBounds, 1.2f);
                    g.setFont (juce::FontOptions (8.0f, juce::Font::bold));
                    g.drawText ("I", powerBounds, juce::Justification::centred, true);
                }

                // Rack Title & Subtitle
                g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
                g.setColour (isBypassed ? lookAndFeel.getMutedSlate() : lookAndFeel.getWhisperWhite());
                g.drawText (meta.title, unitBounds.getX() + 42.0f, unitBounds.getY() + 10.0f, 160.0f, 16.0f, juce::Justification::left, true);

                g.setFont (juce::FontOptions (9.0f));
                g.setColour (lookAndFeel.getMutedSlate().withAlpha (isBypassed ? 0.45f : 0.85f));
                g.drawText (meta.subtitle, unitBounds.getX() + 42.0f, unitBounds.getY() + 28.0f, 160.0f, 14.0f, juce::Justification::left, true);

                // Drag Grip Indicator (three horizontal bars ≡ on right side)
                const float gripX = unitBounds.getRight() - 44.0f;
                const float gripY = unitBounds.getY() + 13.0f;
                g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.45f));
                g.fillRect (gripX, gripY, 12.0f, 2.0f);
                g.fillRect (gripX, gripY + 4.0f, 12.0f, 2.0f);
                g.fillRect (gripX, gripY + 8.0f, 12.0f, 2.0f);

                // Close button [×] on top right
                const float closeX = unitBounds.getRight() - 20.0f;
                const float closeY = unitBounds.getY() + 10.0f;
                g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
                g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.7f));
                g.drawText ("×", closeX, closeY, 14.0f, 14.0f, juce::Justification::centred, true);
            }

            // Draw Drop Insertion Indicator Line
            if (isDragging && dropInsertIndex >= 0)
            {
                const float lineY = b.getY() + dropInsertIndex * unitH;
                const float lineLeft = b.getX() + 4.0f;
                const float lineRight = b.getRight() - 4.0f;

                // Glowing line in Golden Peach (#FEC89A) & Peach Coral (#FEC5BB)
                g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.4f));
                g.fillRect (lineLeft, lineY - 3.0f, lineRight - lineLeft, 6.0f);

                g.setColour (lookAndFeel.getGoldenPeach());
                g.fillRect (lineLeft, lineY - 1.5f, lineRight - lineLeft, 3.0f);

                // Left triangle: ►
                juce::Path leftArrow;
                leftArrow.startNewSubPath (lineLeft, lineY - 6.0f);
                leftArrow.lineTo (lineLeft + 8.0f, lineY);
                leftArrow.lineTo (lineLeft, lineY + 6.0f);
                leftArrow.closeSubPath();
                g.fillPath (leftArrow);

                // Right triangle: ◄
                juce::Path rightArrow;
                rightArrow.startNewSubPath (lineRight, lineY - 6.0f);
                rightArrow.lineTo (lineRight - 8.0f, lineY);
                rightArrow.lineTo (lineRight, lineY + 6.0f);
                rightArrow.closeSubPath();
                g.fillPath (rightArrow);
            }
        }

        void paintSignalFlow (juce::Graphics& g, const juce::Rectangle<float>& b)
        {
            // Title
            g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite());
            g.drawText ("SIGNAL FLOW", b.getX() + 10.0f, b.getY() + 8.0f, b.getWidth() - 20.0f, 18.0f, juce::Justification::left, true);

            // Flow tree blocks matching Guitar Rig right panel
            const float startY = b.getY() + 34.0f;
            const float blockW = 88.0f;
            const float blockH = 18.0f;
            const float cx = b.getCentreX();

            std::vector<std::pair<juce::String, juce::Colour>> flowNodes;
            flowNodes.push_back ({ "L/R IN", lookAndFeel.getSageMint() });

            for (size_t i = 0; i < activeRack.size(); ++i)
            {
                const auto meta = getMetadata (activeRack[i]);
                flowNodes.push_back ({ meta.flowNodeName, activeBypassed[i] ? lookAndFeel.getMutedSlate() : meta.accent });
            }

            flowNodes.push_back ({ "OUT", lookAndFeel.getSageMint() });

            const float stepY = juce::jmin (26.0f, (b.getHeight() - 44.0f) / static_cast<float> (flowNodes.size()));

            for (size_t i = 0; i < flowNodes.size(); ++i)
            {
                const float ny = startY + i * stepY;
                auto nodeRect = juce::Rectangle<float> (cx - blockW * 0.5f, ny, blockW, blockH);

                // Connecting vertical flow line
                if (i < flowNodes.size() - 1)
                {
                    g.setColour (lookAndFeel.getGoldenPeach().withAlpha (0.6f));
                    g.drawVerticalLine (static_cast<int>(cx), ny + blockH, ny + stepY);
                }

                g.setColour (flowNodes[i].second.withAlpha (0.22f));
                g.fillRoundedRectangle (nodeRect, 3.0f);

                g.setColour (flowNodes[i].second);
                g.drawRoundedRectangle (nodeRect, 3.0f, 1.0f);

                g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
                g.setColour (lookAndFeel.getWhisperWhite());
                g.drawText (flowNodes[i].first, nodeRect, juce::Justification::centred, true);
            }
        }

        void paintFloatingGhost (juce::Graphics& g)
        {
            auto ghostBounds = juce::Rectangle<float> (dragPos.x - 90.0f, dragPos.y - 20.0f, 180.0f, 40.0f);
            const auto meta = getMetadata (draggedEffect);

            // Drop shadow
            g.setColour (juce::Colours::black.withAlpha (0.55f));
            g.fillRoundedRectangle (ghostBounds.translated (2.0f, 4.0f), 5.0f);

            // Card body
            g.setColour (juce::Colour::fromRGB (0x2A, 0x2E, 0x32).withAlpha (0.92f));
            g.fillRoundedRectangle (ghostBounds, 5.0f);

            // Accent bar
            g.setColour (meta.accent);
            g.fillRoundedRectangle (ghostBounds.getX() + 3.0f, ghostBounds.getY() + 4.0f, 4.0f, ghostBounds.getHeight() - 8.0f, 2.0f);

            // Glowing Golden Peach border
            g.setColour (lookAndFeel.getGoldenPeach().withAlpha (0.85f));
            g.drawRoundedRectangle (ghostBounds, 5.0f, 1.5f);

            // Title & subtitle
            g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
            g.setColour (lookAndFeel.getWhisperWhite());
            g.drawText (meta.title, ghostBounds.getX() + 16.0f, ghostBounds.getY() + 4.0f, ghostBounds.getWidth() - 20.0f, 16.0f, juce::Justification::left, true);

            g.setFont (juce::FontOptions (8.5f));
            g.setColour (lookAndFeel.getMutedSlate());
            g.drawText (meta.subtitle, ghostBounds.getX() + 16.0f, ghostBounds.getY() + 20.0f, ghostBounds.getWidth() - 20.0f, 14.0f, juce::Justification::left, true);
        }
    };
}
