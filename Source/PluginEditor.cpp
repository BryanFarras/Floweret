#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace Floweret
{
    FloweretAudioProcessorEditor::FloweretAudioProcessorEditor (FloweretAudioProcessor& p)
        : AudioProcessorEditor (&p), audioProcessor (p),
          growthField (lookAndFeel),
          colorKnob ("Color", lookAndFeel),
          mutateKnob ("Mutate", lookAndFeel),
          bloomKnob ("Bloom", lookAndFeel),
          spreadKnob ("Spread", lookAndFeel),
          tailKnob ("Tail", lookAndFeel),
          wavetableDisplay (lookAndFeel),
          grainCloudDisplay (lookAndFeel),
          spectralVisualizer (lookAndFeel, audioProcessor.getSynth().getColorResonator()),
          wtPosKnob ("WT Pos", lookAndFeel),
          wtWarpKnob ("WT Warp", lookAndFeel),
          granSizeKnob ("Grain Size", lookAndFeel),
          granDensityKnob ("Density", lookAndFeel),
          granSprayKnob ("Spray", lookAndFeel),
          specTiltKnob ("Tilt", lookAndFeel),
          specSmearKnob ("Smear", lookAndFeel),
          filterCutoffKnob ("Cutoff", lookAndFeel),
          filterResKnob ("Resonance", lookAndFeel),
          reverbMixKnob ("Reverb", lookAndFeel),
          delayMixKnob ("Delay", lookAndFeel),
          keyboard (lookAndFeel)
    {
        setLookAndFeel (&lookAndFeel);

        // 1. Header Components
        titleLabel.setText ("FLOWERET", juce::dontSendNotification);
        titleLabel.setFont (juce::FontOptions (26.0f, juce::Font::bold));
        titleLabel.setColour (juce::Label::textColourId, lookAndFeel.getDeepMoss());
        addAndMakeVisible (titleLabel);

        guideBanner.setText ("FLOWERET — Botanica Granular & Wavetable Synthesizer for Color Bass, Dubstep & Glitch", juce::dontSendNotification);
        guideBanner.setFont (juce::FontOptions (12.0f));
        guideBanner.setColour (juce::Label::textColourId, juce::Colour::fromRGB (90, 115, 100));
        addAndMakeVisible (guideBanner);

        outputVolumeSlider.setSliderStyle (juce::Slider::LinearHorizontal);
        outputVolumeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (outputVolumeSlider);

        outputVolumeLabel.setText ("0.0 dB", juce::dontSendNotification);
        outputVolumeLabel.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        outputVolumeLabel.setColour (juce::Label::textColourId, lookAndFeel.getDeepMoss());
        addAndMakeVisible (outputVolumeLabel);

        outputVolumeSlider.onValueChange = [this]() {
            outputVolumeLabel.setText (juce::String (outputVolumeSlider.getValue(), 1) + " dB", juce::dontSendNotification);
        };

        voiceCounterLabel.setText ("0 VOICES / 3 SAMPLES", juce::dontSendNotification);
        voiceCounterLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        voiceCounterLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (110, 140, 125));
        addAndMakeVisible (voiceCounterLabel);

        themeButton.setButtonText ("LIGHT");
        addAndMakeVisible (themeButton);

        // 2. Generative Toolbar
        subtitleLabel.setText ("A GENERATIVE BOTANICAL INSTRUMENT", juce::dontSendNotification);
        subtitleLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        subtitleLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (120, 140, 130));
        addAndMakeVisible (subtitleLabel);

        addButton.setButtonText ("ADD");
        clearButton.setButtonText ("CLEAR");
        resetButton.setButtonText ("RESET");
        sproutButton.setButtonText ("SPROUT");

        addAndMakeVisible (addButton);
        addAndMakeVisible (clearButton);
        addAndMakeVisible (resetButton);
        addAndMakeVisible (sproutButton);

        sproutButton.onClick = [this]() { performGenerativeSprout(); };

        keySelector.addItemList (getNoteNames(), 1);
        keySelector.setSelectedId (4, juce::dontSendNotification); // Eb (index 3 + 1)
        addAndMakeVisible (keySelector);

        scaleSelector.addItemList (getScaleNames(), 1);
        scaleSelector.setSelectedId (1, juce::dontSendNotification); // Major
        addAndMakeVisible (scaleSelector);

        // View Tabs
        viewGrowthBtn.setButtonText ("GROWTH FIELD");
        viewWtBtn.setButtonText ("WAVETABLE");
        viewGranBtn.setButtonText ("GRANULAR");
        viewSpecBtn.setButtonText ("SPECTRAL");
        viewFxBtn.setButtonText ("FX & COLOR");
        viewSeqBtn.setButtonText ("SEQUENCER");

        addAndMakeVisible (viewGrowthBtn);
        addAndMakeVisible (viewWtBtn);
        addAndMakeVisible (viewGranBtn);
        addAndMakeVisible (viewSpecBtn);
        addAndMakeVisible (viewFxBtn);
        addAndMakeVisible (viewSeqBtn);

        viewGrowthBtn.onClick = [this]() { switchView (ActiveView::GrowthField); };
        viewWtBtn.onClick     = [this]() { switchView (ActiveView::Wavetable); };
        viewGranBtn.onClick   = [this]() { switchView (ActiveView::Granular); };
        viewSpecBtn.onClick   = [this]() { switchView (ActiveView::Spectral); };
        viewFxBtn.onClick     = [this]() { switchView (ActiveView::Effects); };
        viewSeqBtn.onClick    = [this]() { switchView (ActiveView::Sequencer); };

        // 3. Drop Zones
        oneShotDropLabel.setText ("ONE-SHOT FILES\nDrop Audio Files", juce::dontSendNotification);
        oneShotDropLabel.setJustificationType (juce::Justification::centred);
        oneShotDropLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        oneShotDropLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (110, 130, 120));
        addAndMakeVisible (oneShotDropLabel);

        loopDropLabel.setText ("LOOP FILES / SFZ\n3 Active Banks Loaded", juce::dontSendNotification);
        loopDropLabel.setJustificationType (juce::Justification::centred);
        loopDropLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        loopDropLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (110, 130, 120));
        addAndMakeVisible (loopDropLabel);

        // 4. Growth Field Center & Knobs
        addAndMakeVisible (growthField);
        addAndMakeVisible (colorKnob);
        addAndMakeVisible (mutateKnob);
        addAndMakeVisible (bloomKnob);
        addAndMakeVisible (spreadKnob);
        addAndMakeVisible (tailKnob);

        // Secondary displays & knobs
        addChildComponent (wavetableDisplay);
        addChildComponent (grainCloudDisplay);
        addChildComponent (spectralVisualizer);

        addChildComponent (wtPosKnob);
        addChildComponent (wtWarpKnob);
        addChildComponent (granSizeKnob);
        addChildComponent (granDensityKnob);
        addChildComponent (granSprayKnob);
        addChildComponent (specTiltKnob);
        addChildComponent (specSmearKnob);
        addChildComponent (filterCutoffKnob);
        addChildComponent (filterResKnob);
        addChildComponent (reverbMixKnob);
        addChildComponent (delayMixKnob);

        // 5. Bottom Keyboard
        addAndMakeVisible (keyboard);
        keyboard.onNoteOn = [this](int midiNote, float vel) {
            audioProcessor.getSynth().handleNoteOn (midiNote, vel);
        };
        keyboard.onNoteOff = [this](int midiNote, float vel) {
            audioProcessor.getSynth().handleNoteOff (midiNote, vel);
        };

        setupAttachments();

        setSize (980, 640);
        startTimerHz (30);
    }

    FloweretAudioProcessorEditor::~FloweretAudioProcessorEditor()
    {
        stopTimer();
        setLookAndFeel (nullptr);
    }

    void FloweretAudioProcessorEditor::setupAttachments()
    {
        auto& apvts = audioProcessor.getAPVTS();

        // Sliders
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::masterGain.getParamID(), outputVolumeSlider));

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::colorAmount.getParamID(), colorKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::mutateAmount.getParamID(), mutateKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::bloomAmount.getParamID(), bloomKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::spreadAmount.getParamID(), spreadKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::tailAmount.getParamID(), tailKnob.getSlider()));

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::wtPosition.getParamID(), wtPosKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::wtWarpAmount.getParamID(), wtWarpKnob.getSlider()));

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::granSize.getParamID(), granSizeKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::granDensity.getParamID(), granDensityKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::granSpray.getParamID(), granSprayKnob.getSlider()));

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::specTilt.getParamID(), specTiltKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::specSmear.getParamID(), specSmearKnob.getSlider()));

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::filterCutoff.getParamID(), filterCutoffKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::filterResonance.getParamID(), filterResKnob.getSlider()));

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::reverbMix.getParamID(), reverbMixKnob.getSlider()));
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::delayMix.getParamID(), delayMixKnob.getSlider()));

        // Combos
        comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
            apvts, ParamIDs::colorKey.getParamID(), keySelector));
        comboAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
            apvts, ParamIDs::colorScale.getParamID(), scaleSelector));
    }

    void FloweretAudioProcessorEditor::switchView (ActiveView newView)
    {
        currentView = newView;

        const bool isGrowth = (currentView == ActiveView::GrowthField);
        const bool isWt     = (currentView == ActiveView::Wavetable);
        const bool isGran   = (currentView == ActiveView::Granular);
        const bool isSpec   = (currentView == ActiveView::Spectral);
        const bool isFx     = (currentView == ActiveView::Effects);

        growthField.setVisible (isGrowth);
        colorKnob.setVisible (isGrowth || isFx);
        mutateKnob.setVisible (isGrowth);
        bloomKnob.setVisible (isGrowth);
        spreadKnob.setVisible (isGrowth);
        tailKnob.setVisible (isGrowth);

        wavetableDisplay.setVisible (isWt);
        wtPosKnob.setVisible (isWt);
        wtWarpKnob.setVisible (isWt);

        grainCloudDisplay.setVisible (isGran);
        granSizeKnob.setVisible (isGran);
        granDensityKnob.setVisible (isGran);
        granSprayKnob.setVisible (isGran);

        spectralVisualizer.setVisible (isSpec || isFx);
        specTiltKnob.setVisible (isSpec);
        specSmearKnob.setVisible (isSpec);

        filterCutoffKnob.setVisible (isFx);
        filterResKnob.setVisible (isFx);
        reverbMixKnob.setVisible (isFx);
        delayMixKnob.setVisible (isFx);

        resized();
        repaint();
    }

    void FloweretAudioProcessorEditor::performGenerativeSprout()
    {
        // Sprout: Generatively mutate parameters with harmonious musical offsets
        FastRandom rng;
        auto& apvts = audioProcessor.getAPVTS();

        if (auto* p = apvts.getParameter (ParamIDs::wtPosition.getParamID()))
            p->setValueNotifyingHost (rng.nextUnsignedFloat());

        if (auto* p = apvts.getParameter (ParamIDs::granPosition.getParamID()))
            p->setValueNotifyingHost (rng.nextUnsignedFloat());

        if (auto* p = apvts.getParameter (ParamIDs::colorAmount.getParamID()))
            p->setValueNotifyingHost (0.15f + rng.nextUnsignedFloat() * 0.7f);

        if (auto* p = apvts.getParameter (ParamIDs::bloomAmount.getParamID()))
            p->setValueNotifyingHost (0.1f + rng.nextUnsignedFloat() * 0.5f);
    }

    void FloweretAudioProcessorEditor::timerCallback()
    {
        // Update voice and sample counters
        const int voiceCount = audioProcessor.getSynth().getActiveVoiceCount();
        voiceCounterLabel.setText (juce::String (voiceCount) + " VOICES / 3 SAMPLES", juce::dontSendNotification);

        // Update real-time visualizer inputs
        growthField.setAudioActivity (static_cast<float> (voiceCount) * 0.35f);

        if (currentView == ActiveView::Wavetable)
        {
            const float pos = audioProcessor.getAPVTS().getRawParameterValue (ParamIDs::wtPosition.getParamID())->load();
            wavetableDisplay.updateWaveform (pos);
        }
        else if (currentView == ActiveView::Granular)
        {
            const float pos = audioProcessor.getAPVTS().getRawParameterValue (ParamIDs::granPosition.getParamID())->load();
            const float spray = audioProcessor.getAPVTS().getRawParameterValue (ParamIDs::granSpray.getParamID())->load();
            grainCloudDisplay.updateParams (pos, spray);
        }
    }

    void FloweretAudioProcessorEditor::paint (juce::Graphics& g)
    {
        // Fill pristine warm ivory background
        g.fillAll (lookAndFeel.getIvoryBg());

        // Header bottom divider line
        g.setColour (lookAndFeel.getCardBorder());
        g.drawHorizontalLine (56, 16.0f, static_cast<float> (getWidth() - 16));

        // Draw frosted panels behind drop zones
        auto dropBounds = juce::Rectangle<float> (16.0f, 66.0f, static_cast<float> (getWidth() - 32), 62.0f);
        g.setColour (juce::Colour::fromRGB (248, 247, 243));
        g.fillRoundedRectangle (dropBounds, 8.0f);
        g.setColour (lookAndFeel.getCardBorder());
        g.drawRoundedRectangle (dropBounds, 8.0f, 1.0f);
        g.drawVerticalLine (static_cast<int> (dropBounds.getCentreX()), dropBounds.getY(), dropBounds.getBottom());
    }

    void FloweretAudioProcessorEditor::resized()
    {
        const int w = getWidth();
        const int h = getHeight();

        // 1. Header Bar (y: 12 to 50)
        titleLabel.setBounds (20, 10, 160, 34);
        guideBanner.setBounds (170, 16, 420, 24);

        outputVolumeSlider.setBounds (w - 380, 18, 90, 22);
        outputVolumeLabel.setBounds (w - 285, 18, 55, 22);
        voiceCounterLabel.setBounds (w - 225, 18, 140, 22);
        themeButton.setBounds (w - 80, 14, 64, 28);

        // 2. Toolbar & Drop Zones (y: 64 to 126)
        const int dropW = (w - 36) / 2;
        oneShotDropLabel.setBounds (18, 68, dropW - 4, 58);
        loopDropLabel.setBounds (18 + dropW + 4, 68, dropW - 4, 58);

        // Toolbar Action Pills
        subtitleLabel.setBounds (22, 136, 260, 24);
        addButton.setBounds (w - 480, 134, 52, 26);
        clearButton.setBounds (w - 422, 134, 58, 26);
        resetButton.setBounds (w - 358, 134, 58, 26);
        sproutButton.setBounds (w - 294, 134, 68, 26);
        keySelector.setBounds (w - 220, 134, 75, 26);
        scaleSelector.setBounds (w - 140, 134, 120, 26);

        // View Tabs (y: 168 to 196)
        const int tabW = (w - 32) / 6;
        viewGrowthBtn.setBounds (16 + 0 * tabW, 168, tabW - 4, 26);
        viewWtBtn.setBounds     (16 + 1 * tabW, 168, tabW - 4, 26);
        viewGranBtn.setBounds   (16 + 2 * tabW, 168, tabW - 4, 26);
        viewSpecBtn.setBounds   (16 + 3 * tabW, 168, tabW - 4, 26);
        viewFxBtn.setBounds     (16 + 4 * tabW, 168, tabW - 4, 26);
        viewSeqBtn.setBounds    (16 + 5 * tabW, 168, tabW - 4, 26);

        // 3. Central Canvas (y: 202 to h - 90)
        const int canvasY = 202;
        const int canvasH = h - canvasY - 94;
        const auto centerArea = juce::Rectangle<int> (16, canvasY, w - 32, canvasH);

        growthField.setBounds (centerArea);

        // Place Dials across the Growth Field matching VINE mockup
        const int knobSize = 100;
        const int dialY = canvasY + (canvasH / 2) - 30;

        colorKnob.setBounds  (centerArea.getX() + static_cast<int> (centerArea.getWidth() * 0.16) - knobSize / 2, dialY + 20, knobSize, knobSize + 15);
        mutateKnob.setBounds (centerArea.getX() + static_cast<int> (centerArea.getWidth() * 0.30) - knobSize / 2, dialY - 50, knobSize, knobSize + 15);
        bloomKnob.setBounds  (centerArea.getX() + static_cast<int> (centerArea.getWidth() * 0.50) - knobSize / 2, dialY + 30, knobSize, knobSize + 15);
        spreadKnob.setBounds (centerArea.getX() + static_cast<int> (centerArea.getWidth() * 0.70) - knobSize / 2, dialY - 50, knobSize, knobSize + 15);
        tailKnob.setBounds   (centerArea.getX() + static_cast<int> (centerArea.getWidth() * 0.84) - knobSize / 2, dialY + 25, knobSize, knobSize + 15);

        // Engine Displays
        const auto displayBounds = centerArea.reduced (20);
        wavetableDisplay.setBounds (displayBounds.withTrimmedRight (centerArea.getWidth() / 3));
        grainCloudDisplay.setBounds (displayBounds.withTrimmedRight (centerArea.getWidth() / 3));
        spectralVisualizer.setBounds (displayBounds.withTrimmedRight (centerArea.getWidth() / 3));

        // Side Knobs for active engine views
        const int sideX = displayBounds.getRight() - (centerArea.getWidth() / 3) + 20;
        wtPosKnob.setBounds (sideX, canvasY + 20, knobSize, knobSize + 15);
        wtWarpKnob.setBounds (sideX + knobSize + 20, canvasY + 20, knobSize, knobSize + 15);

        granSizeKnob.setBounds (sideX, canvasY + 20, knobSize, knobSize + 15);
        granDensityKnob.setBounds (sideX + knobSize + 10, canvasY + 20, knobSize, knobSize + 15);
        granSprayKnob.setBounds (sideX, canvasY + knobSize + 40, knobSize, knobSize + 15);

        specTiltKnob.setBounds (sideX, canvasY + 20, knobSize, knobSize + 15);
        specSmearKnob.setBounds (sideX + knobSize + 10, canvasY + 20, knobSize, knobSize + 15);

        filterCutoffKnob.setBounds (sideX, canvasY + 20, knobSize, knobSize + 15);
        filterResKnob.setBounds (sideX + knobSize + 10, canvasY + 20, knobSize, knobSize + 15);
        reverbMixKnob.setBounds (sideX, canvasY + knobSize + 40, knobSize, knobSize + 15);
        delayMixKnob.setBounds (sideX + knobSize + 10, canvasY + knobSize + 40, knobSize, knobSize + 15);

        // 4. Bottom Virtual Keyboard (y: h - 86 to h - 6)
        keyboard.setBounds (16, h - 86, w - 32, 80);
    }
}
