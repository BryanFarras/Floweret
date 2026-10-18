#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace Floweret
{
    FloweretAudioProcessorEditor::FloweretAudioProcessorEditor (FloweretAudioProcessor& p)
        : AudioProcessorEditor (&p), audioProcessor (p),
          oscA (OscId::A, lookAndFeel, audioProcessor.getAPVTS()),
          oscB (OscId::B, lookAndFeel, audioProcessor.getAPVTS()),
          oscC (OscId::C, lookAndFeel, audioProcessor.getAPVTS()),
          noiseSub (lookAndFeel, audioProcessor.getAPVTS()),
          preFilter (lookAndFeel, audioProcessor.getAPVTS()),
          voicing (lookAndFeel, audioProcessor.getAPVTS()),
          envModule (lookAndFeel, audioProcessor.getAPVTS()),
          lfoModule (lookAndFeel, audioProcessor.getAPVTS()),
          emptyBoxRight (lookAndFeel, "[ EMPTY BOX ]"),
          guitarRigRack (lookAndFeel, audioProcessor.getAPVTS()),
          samplerDisplay (lookAndFeel),
          routesDisplay (lookAndFeel, "[ SIGNAL ROUTING MATRIX ]"),
          keyboard (lookAndFeel)
    {
        setLookAndFeel (&lookAndFeel);

        // 1. Top Switchable View Tabs from design1.png
        oscTabBtn.setButtonText ("OSCILATOR");
        effectsTabBtn.setButtonText ("EFFECTS");
        routesTabBtn.setButtonText ("ROUTES");
        samplerTabBtn.setButtonText ("SAMPLER");

        oscTabBtn.setToggleState (true, juce::dontSendNotification);

        addAndMakeVisible (oscTabBtn);
        addAndMakeVisible (effectsTabBtn);
        addAndMakeVisible (routesTabBtn);
        addAndMakeVisible (samplerTabBtn);

        oscTabBtn.onClick     = [this]() { switchView (ActiveView::Oscillator); };
        effectsTabBtn.onClick = [this]() { switchView (ActiveView::Effects); };
        routesTabBtn.onClick  = [this]() { switchView (ActiveView::Routes); };
        samplerTabBtn.onClick = [this]() { switchView (ActiveView::Sampler); };

        // Top Right Master Controls
        masterLabel.setText ("MASTER", juce::dontSendNotification);
        masterLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        masterLabel.setColour (juce::Label::textColourId, lookAndFeel.getMutedSlate());
        addAndMakeVisible (masterLabel);

        masterGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
        masterGainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (masterGainSlider);

        masterReadoutLabel.setText ("0.0 dB", juce::dontSendNotification);
        masterReadoutLabel.setFont (juce::FontOptions (10.5f, juce::Font::bold));
        masterReadoutLabel.setColour (juce::Label::textColourId, lookAndFeel.getDarkEspresso());
        addAndMakeVisible (masterReadoutLabel);

        masterGainSlider.onValueChange = [this]() {
            masterReadoutLabel.setText (juce::String (masterGainSlider.getValue(), 1) + " dB", juce::dontSendNotification);
        };

        voiceCounterLabel.setText ("0 VOICES / 3 SAMPLES", juce::dontSendNotification);
        voiceCounterLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        voiceCounterLabel.setColour (juce::Label::textColourId, lookAndFeel.getMutedSlate());
        voiceCounterLabel.setJustificationType (juce::Justification::centredRight);
        addAndMakeVisible (voiceCounterLabel);

        sproutButton.setButtonText ("SPROUT");
        addAndMakeVisible (sproutButton);
        sproutButton.onClick = [this]() { performGenerativeSprout(); };

        // 2. OSCILATOR Tab Modules (from design1.png)
        addAndMakeVisible (oscA);
        addAndMakeVisible (oscB);
        addAndMakeVisible (oscC);
        addAndMakeVisible (noiseSub);
        addAndMakeVisible (preFilter);

        addAndMakeVisible (voicing);
        addAndMakeVisible (envModule);
        addAndMakeVisible (lfoModule);
        addAndMakeVisible (emptyBoxRight);

        // 3. Alternative Views
        addChildComponent (guitarRigRack);
        addChildComponent (samplerDisplay);
        addChildComponent (routesDisplay);

        guitarRigRack.onOrderChanged = [this](const std::vector<int>& chain) {
            audioProcessor.getSynth().setEffectChain (chain);
        };

        // 4. Bottom Virtual Piano Keyboard (from design1.png)
        addAndMakeVisible (keyboard);
        keyboard.onNoteOn = [this](int midiNote, float vel) {
            audioProcessor.getSynth().handleNoteOn (midiNote, vel);
        };
        keyboard.onNoteOff = [this](int midiNote, float vel) {
            audioProcessor.getSynth().handleNoteOff (midiNote, vel);
        };

        setupAttachments();

        logoImage = LogoData::getLogoImage();

        setResizable (true, true);
        setResizeLimits (900, 605, 2640, 1776);
        getConstrainer()->setFixedAspectRatio (1320.0 / 888.0);
        setSize (1320, 888);
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

        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, ParamIDs::masterGain.getParamID(), masterGainSlider));
    }

    void FloweretAudioProcessorEditor::switchView (ActiveView newView)
    {
        currentView = newView;

        oscTabBtn.setToggleState     (currentView == ActiveView::Oscillator, juce::dontSendNotification);
        effectsTabBtn.setToggleState (currentView == ActiveView::Effects, juce::dontSendNotification);
        routesTabBtn.setToggleState  (currentView == ActiveView::Routes, juce::dontSendNotification);
        samplerTabBtn.setToggleState (currentView == ActiveView::Sampler, juce::dontSendNotification);

        const bool isOsc = (currentView == ActiveView::Oscillator);

        oscA.setVisible (isOsc);
        oscB.setVisible (isOsc);
        oscC.setVisible (isOsc);
        noiseSub.setVisible (isOsc);
        preFilter.setVisible (isOsc);

        voicing.setVisible (isOsc);
        envModule.setVisible (isOsc);
        lfoModule.setVisible (isOsc);
        emptyBoxRight.setVisible (isOsc);

        guitarRigRack.setVisible  (currentView == ActiveView::Effects);
        routesDisplay.setVisible  (currentView == ActiveView::Routes);
        samplerDisplay.setVisible (currentView == ActiveView::Sampler);

        resized();
        repaint();
    }

    void FloweretAudioProcessorEditor::performGenerativeSprout()
    {
        FastRandom rng;
        auto& apvts = audioProcessor.getAPVTS();

        if (auto* p = apvts.getParameter (ParamIDs::wtPosition.getParamID()))
            p->setValueNotifyingHost (rng.nextUnsignedFloat());

        if (auto* p = apvts.getParameter (ParamIDs::filterCutoff.getParamID()))
            p->setValueNotifyingHost (0.15f + rng.nextUnsignedFloat() * 0.7f);

        if (auto* p = apvts.getParameter (ParamIDs::colorAmount.getParamID()))
            p->setValueNotifyingHost (0.2f + rng.nextUnsignedFloat() * 0.6f);

        if (auto* p = apvts.getParameter (ParamIDs::bloomAmount.getParamID()))
            p->setValueNotifyingHost (0.1f + rng.nextUnsignedFloat() * 0.5f);
    }

    void FloweretAudioProcessorEditor::timerCallback()
    {
        const int voiceCount = audioProcessor.getSynth().getActiveVoiceCount();
        voiceCounterLabel.setText (juce::String (voiceCount) + " VOICES / 3 SAMPLES", juce::dontSendNotification);
    }

    void FloweretAudioProcessorEditor::paint (juce::Graphics& g)
    {
        // 1. Warm Oatmeal Background (#ECE4DB)
        g.fillAll (lookAndFeel.getOatmealCanvas());

        const float w = static_cast<float> (getWidth());
        const float h = static_cast<float> (getHeight());

        // 2. Subtle Blueprint / Drafting Dot Grid from design1.png
        g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.45f));
        const float dotSpacing = 16.0f;
        const float bottomGridY = static_cast<float> (keyboard.getY() - 4);
        for (float y = 42.0f; y < bottomGridY; y += dotSpacing)
        {
            for (float x = 12.0f; x < w - 12.0f; x += dotSpacing)
            {
                g.fillEllipse (x - 1.0f, y - 1.0f, 2.0f, 2.0f);
            }
        }

        // Header separator line
        g.setColour (lookAndFeel.getPlatinumBorder());
        g.drawHorizontalLine (38, 12.0f, w - 12.0f);

        // 3. Brand Title / Logo in the top-middle next to tabs
        if (logoImage.isValid())
        {
            const float maxLogoW = juce::jmax (120.0f, (w - 370.0f) - 384.0f);
            const float logoH = 26.0f;
            const float logoW = juce::jmin (242.0f, maxLogoW);
            const float logoX = juce::jmax (384.0f, (w - logoW) * 0.5f);
            const float logoY = 6.0f;

            g.drawImageWithin (logoImage,
                               static_cast<int> (logoX), static_cast<int> (logoY),
                               static_cast<int> (logoW), static_cast<int> (logoH),
                               juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
        }

        // Drag-and-drop indicator
        if (isDraggingFile)
        {
            g.setColour (lookAndFeel.getPeachCoral().withAlpha (0.4f));
            g.drawRect (getLocalBounds().toFloat(), 4.0f);
        }
    }

    void FloweretAudioProcessorEditor::resized()
    {
        const int w = getWidth();
        const int h = getHeight();

        // 1. Top Switchable Tabs from design1.png (OSCILATOR, EFFECTS, ROUTES, SAMPLER)
        const int tabW = 86;
        const int tabH = 26;
        const int tabY = 6;

        oscTabBtn.setBounds     (12 + 0 * (tabW + 6), tabY, tabW, tabH);
        effectsTabBtn.setBounds (12 + 1 * (tabW + 6), tabY, tabW, tabH);
        routesTabBtn.setBounds  (12 + 2 * (tabW + 6), tabY, tabW, tabH);
        samplerTabBtn.setBounds (12 + 3 * (tabW + 6), tabY, tabW, tabH);

        // Top Right Master Controls
        masterLabel.setBounds        (w - 360, tabY + 4, 52, 18);
        masterGainSlider.setBounds    (w - 306, tabY + 4, 85, 18);
        masterReadoutLabel.setBounds  (w - 216, tabY + 4, 48, 18);
        voiceCounterLabel.setBounds   (w - 162, tabY + 4, 88, 18);
        sproutButton.setBounds       (w - 68,  tabY,     56, tabH);

        // 2. Main Workspace Layout from design1.png (y: 44 to 644)
        const int marginX = 12;
        const int totalW = w - 24;
        const int colGap = 12;

        const int leftColW = (totalW - colGap) * 48 / 100;
        const int rightColW = totalW - colGap - leftColW;
        const int leftColX = marginX;
        const int rightColX = leftColX + leftColW + colGap;

        // Bottom Strip: Full-Width Virtual Piano Keyboard
        const int keyboardH = juce::jlimit (60, 110, static_cast<int> (h * 0.11f));
        const int keyboardY = h - keyboardH - 8;
        const int workspaceY = 42;
        const int workspaceH = keyboardY - workspaceY - 8;

        // Left Column:
        // Row 1: OSC A
        // Row 2: OSC B
        // Row 3: OSC C
        // Row 4: NOISE OSC & SUB OSC
        // Row 5: PRE FILTER (3) & [EMPTY BOX]
        const int rowGap = juce::jlimit (4, 10, static_cast<int> (workspaceH * 0.012f));
        const int availLeftH = workspaceH - (4 * rowGap);
        const int oscH = static_cast<int> (availLeftH * 0.185f);
        const int noiseH = static_cast<int> (availLeftH * 0.13f);
        const int filterH = availLeftH - (3 * oscH) - noiseH;

        oscA.setBounds (leftColX, workspaceY + 0 * (oscH + rowGap), leftColW, oscH);
        oscB.setBounds (leftColX, workspaceY + 1 * (oscH + rowGap), leftColW, oscH);
        oscC.setBounds (leftColX, workspaceY + 2 * (oscH + rowGap), leftColW, oscH);

        const int noiseY = workspaceY + 3 * (oscH + rowGap);
        noiseSub.setBounds (leftColX, noiseY, leftColW, noiseH);

        const int filterY = noiseY + noiseH + rowGap;
        preFilter.setBounds (leftColX, filterY, leftColW, filterH);

        // Right Column:
        // Row 1: VOICING & ENV
        // Row 2: LFO (4 LFO Default)
        // Row 3: [EMPTY BOX]
        const int availRightH = workspaceH - (2 * rowGap);
        const int rRow1H = static_cast<int> (availRightH * 0.30f);
        const int lfoH = static_cast<int> (availRightH * 0.37f);
        const int rEmptyH = availRightH - rRow1H - lfoH;

        const int rHalfW = (rightColW - 6) / 2;
        voicing.setBounds   (rightColX, workspaceY, rHalfW, rRow1H);
        envModule.setBounds (rightColX + rHalfW + 6, workspaceY, rightColW - rHalfW - 6, rRow1H);

        const int lfoY = workspaceY + rRow1H + rowGap;
        lfoModule.setBounds (rightColX, lfoY, rightColW, lfoH);

        const int rEmptyY = lfoY + lfoH + rowGap;
        emptyBoxRight.setBounds (rightColX, rEmptyY, rightColW, rEmptyH);

        // Alternative Views (Cover main workspace when active)
        const auto altBounds = juce::Rectangle<int> (marginX, workspaceY, totalW, workspaceH);
        guitarRigRack.setBounds  (altBounds);
        samplerDisplay.setBounds (altBounds);
        routesDisplay.setBounds  (altBounds);

        // 3. Bottom Strip: Full-Width Virtual Piano Keyboard from design1.png
        keyboard.setBounds (marginX, keyboardY, totalW, keyboardH);
    }

    bool FloweretAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray& files)
    {
        for (const auto& file : files)
        {
            const auto ext = juce::File (file).getFileExtension().toLowerCase();
            if (ext == ".wav" || ext == ".aif" || ext == ".aiff" || ext == ".mp3" || ext == ".flac" || ext == ".ogg")
                return true;
        }
        return false;
    }

    void FloweretAudioProcessorEditor::fileDragEnter (const juce::StringArray& files, int x, int y)
    {
        juce::ignoreUnused (files, x, y);
        isDraggingFile = true;
        repaint();
    }

    void FloweretAudioProcessorEditor::fileDragMove (const juce::StringArray& files, int x, int y)
    {
        juce::ignoreUnused (files, x, y);
    }

    void FloweretAudioProcessorEditor::fileDragExit (const juce::StringArray& files)
    {
        juce::ignoreUnused (files);
        isDraggingFile = false;
        repaint();
    }

    void FloweretAudioProcessorEditor::filesDropped (const juce::StringArray& files, int x, int y)
    {
        juce::ignoreUnused (x, y);
        isDraggingFile = false;
        repaint();

        if (files.isEmpty())
            return;

        const juce::File file (files[0]);
        if (!file.existsAsFile())
            return;

        const bool isLoop = (currentView == ActiveView::Sampler);
        if (audioProcessor.loadAudioFile (file, isLoop))
        {
            samplerDisplay.setSampleName (file.getFileNameWithoutExtension());
            voiceCounterLabel.setText ("SAMPLE: " + file.getFileName().substring (0, 12), juce::dontSendNotification);
            repaint();
        }
    }
}
