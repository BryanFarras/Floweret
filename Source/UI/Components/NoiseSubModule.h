#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"
#include "../../Common/Parameters.h"

namespace Floweret
{
    class NoiseSubModule : public juce::Component
    {
    public:
        NoiseSubModule (BotanicaLookAndFeel& laf, juce::AudioProcessorValueTreeState& apvts)
            : lookAndFeel (laf)
        {
            // Noise Section
            noiseTitle.setText ("NOISE OSC", juce::dontSendNotification);
            noiseTitle.setFont (juce::FontOptions (10.5f, juce::Font::bold));
            noiseTitle.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (noiseTitle);

            noisePowerBtn.setButtonText ("I");
            noisePowerBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (noisePowerBtn);

            noiseColorBox.addItemList (getNoiseColorNames(), 1);
            noiseColorBox.setSelectedId (static_cast<int>(NoiseColor::Velvet) + 1, juce::dontSendNotification);
            addAndMakeVisible (noiseColorBox);

            setupDial (noiseLevelSlider, noiseLevelLabel, "LEVEL");

            // Sub Section
            subTitle.setText ("SUB OSC", juce::dontSendNotification);
            subTitle.setFont (juce::FontOptions (10.5f, juce::Font::bold));
            subTitle.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
            addAndMakeVisible (subTitle);

            subPowerBtn.setButtonText ("I");
            subPowerBtn.setToggleState (true, juce::dontSendNotification);
            addAndMakeVisible (subPowerBtn);

            subOctaveBox.addItemList ({ "-1 Oct", "-2 Oct", "-3 Oct" }, 1);
            subOctaveBox.setSelectedId (1, juce::dontSendNotification);
            addAndMakeVisible (subOctaveBox);

            setupDial (subDriveSlider, subDriveLabel, "DRIVE");
            setupDial (subLevelSlider, subLevelLabel, "LEVEL");

            // Attachments
            buttonAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                apvts, ParamIDs::noiseActive.getParamID(), noisePowerBtn));
            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, ParamIDs::noiseColor.getParamID(), noiseColorBox));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::noiseLevel.getParamID(), noiseLevelSlider));

            buttonAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                apvts, ParamIDs::subActive.getParamID(), subPowerBtn));
            comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, ParamIDs::subOctave.getParamID(), subOctaveBox));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::subDrive.getParamID(), subDriveSlider));
            sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, ParamIDs::subLevel.getParamID(), subLevelSlider));
        }

        ~NoiseSubModule() override = default;

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);
            const float midX = bounds.getCentreX();

            // Left card: Noise
            auto leftBounds = juce::Rectangle<float> (bounds.getX(), bounds.getY(), midX - bounds.getX() - 3.0f, bounds.getHeight());
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (leftBounds, 5.0f);
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (leftBounds, 5.0f, 1.0f);
            g.drawHorizontalLine (24, leftBounds.getX(), leftBounds.getRight());

            // Right card: Sub
            auto rightBounds = juce::Rectangle<float> (midX + 3.0f, bounds.getY(), bounds.getRight() - midX - 3.0f, bounds.getHeight());
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (rightBounds, 5.0f);
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (rightBounds, 5.0f, 1.0f);
            g.drawHorizontalLine (24, rightBounds.getX(), rightBounds.getRight());
        }

        void resized() override
        {
            const int midX = getWidth() / 2;

            // Left: Noise
            noisePowerBtn.setBounds (6, 4, 18, 18);
            noiseTitle.setBounds (28, 4, 70, 18);
            noiseColorBox.setBounds (6, 32, midX - 58, 22);

            const int dialSize = 34;
            noiseLevelLabel.setBounds (midX - 48, 26, 44, 14);
            noiseLevelSlider.setBounds (midX - 44, 40, dialSize, dialSize);

            // Right: Sub
            const int rX = midX + 4;
            subPowerBtn.setBounds (rX + 4, 4, 18, 18);
            subTitle.setBounds (rX + 26, 4, 60, 18);
            subOctaveBox.setBounds (rX + 4, 32, 68, 22);

            const int subDialW = 44;
            subDriveLabel.setBounds (rX + 78, 26, subDialW, 14);
            subDriveSlider.setBounds (rX + 83, 40, dialSize, dialSize);

            subLevelLabel.setBounds (rX + 124, 26, subDialW, 14);
            subLevelSlider.setBounds (rX + 129, 40, dialSize, dialSize);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;

        juce::Label noiseTitle, subTitle;
        juce::TextButton noisePowerBtn, subPowerBtn;
        juce::ComboBox noiseColorBox, subOctaveBox;
        juce::Slider noiseLevelSlider, subDriveSlider, subLevelSlider;
        juce::Label noiseLevelLabel, subDriveLabel, subLevelLabel;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;

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
