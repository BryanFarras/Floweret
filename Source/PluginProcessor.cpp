#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace Floweret
{
    FloweretAudioProcessor::FloweretAudioProcessor()
        : AudioProcessor (BusesProperties()
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
          apvts (*this, nullptr, "Parameters", createParameterLayout())
    {
        formatManager.registerBasicFormats();
    }

    FloweretAudioProcessor::~FloweretAudioProcessor()
    {
    }

    const juce::String FloweretAudioProcessor::getName() const
    {
        return "Floweret";
    }

    bool FloweretAudioProcessor::acceptsMidi() const
    {
        return true;
    }

    bool FloweretAudioProcessor::producesMidi() const
    {
        return true;
    }

    bool FloweretAudioProcessor::isMidiEffect() const
    {
        return false;
    }

    double FloweretAudioProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }

    int FloweretAudioProcessor::getNumPrograms()
    {
        return 1;
    }

    int FloweretAudioProcessor::getCurrentProgram()
    {
        return 0;
    }

    void FloweretAudioProcessor::setCurrentProgram (int index)
    {
        juce::ignoreUnused (index);
    }

    const juce::String FloweretAudioProcessor::getProgramName (int index)
    {
        juce::ignoreUnused (index);
        return {};
    }

    void FloweretAudioProcessor::changeProgramName (int index, const juce::String& newName)
    {
        juce::ignoreUnused (index, newName);
    }

    void FloweretAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
    {
        synth.prepare (sampleRate, samplesPerBlock);
    }

    void FloweretAudioProcessor::releaseResources()
    {
        synth.reset();
    }

    bool FloweretAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
    {
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return true;
    }

    void FloweretAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        juce::ScopedNoDenormals noDenormals;

        // Clear output buffer
        buffer.clear();

        // Get BPM from Host Playhead
        double bpm = 120.0;
        if (auto* playHead = getPlayHead())
        {
            if (auto pos = playHead->getPosition())
            {
                if (pos->getBpm().hasValue())
                    bpm = *pos->getBpm();
            }
        }

        // Process audio block through Floweret synthesizer
        synth.processBlock (buffer, midiMessages, apvts, bpm);

        // Safety Sanitizer: prevent NaNs, Infs, or extreme spikes from poisoning the host mixer
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* channelData = buffer.getWritePointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                const float s = channelData[i];
                if (std::isnan (s) || std::isinf (s))
                {
                    channelData[i] = 0.0f;
                }
                else
                {
                    channelData[i] = juce::jlimit (-2.0f, 2.0f, s);
                }
            }
        }
    }

    bool FloweretAudioProcessor::hasEditor() const
    {
        return true;
    }

    juce::AudioProcessorEditor* FloweretAudioProcessor::createEditor()
    {
        return new FloweretAudioProcessorEditor (*this);
    }

    void FloweretAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
    {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }

    void FloweretAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
    {
        std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }

    bool FloweretAudioProcessor::loadAudioFile (const juce::File& file, bool isLoopOrGranular)
    {
        if (!file.existsAsFile())
            return false;

        std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
        if (reader == nullptr)
            return false;

        const int numSamples = static_cast<int> (reader->lengthInSamples);
        if (numSamples <= 0)
            return false;

        // Cap at 30 seconds of audio to keep RAM lightweight
        const int maxSamples = static_cast<int> (reader->sampleRate * 30.0);
        const int samplesToRead = std::min (numSamples, maxSamples);

        juce::AudioBuffer<float> newBuffer (1, samplesToRead);
        reader->read (&newBuffer, 0, samplesToRead, 0, true, true);

        // Peak normalization
        const float maxMag = newBuffer.getMagnitude (0, samplesToRead);
        if (maxMag > 0.001f)
        {
            newBuffer.applyGain (0.85f / maxMag);
        }

        synth.loadSample (newBuffer, isLoopOrGranular);
        lastLoadedFileName = file.getFileName();
        lastLoadedWasLoop = isLoopOrGranular;

        // Automatically activate the corresponding oscillator parameter
        if (isLoopOrGranular)
        {
            if (auto* p = apvts.getParameter (ParamIDs::granActive.getParamID()))
                p->setValueNotifyingHost (1.0f);
        }
        else
        {
            if (auto* p = apvts.getParameter (ParamIDs::smpActive.getParamID()))
                p->setValueNotifyingHost (1.0f);
        }

        return true;
    }
}

//==============================================================================
// JUCE Audio Plugin Entry Point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Floweret::FloweretAudioProcessor();
}
