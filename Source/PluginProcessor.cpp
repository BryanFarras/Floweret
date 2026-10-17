#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace Floweret
{
    FloweretAudioProcessor::FloweretAudioProcessor()
        : AudioProcessor (BusesProperties()
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
          apvts (*this, nullptr, "Parameters", createParameterLayout())
    {
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
}

//==============================================================================
// JUCE Audio Plugin Entry Point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Floweret::FloweretAudioProcessor();
}
