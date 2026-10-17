#pragma once

#include <JuceHeader.h>
#include "DSP/FloweretSynth.h"
#include "Common/Parameters.h"

namespace Floweret
{
    class FloweretAudioProcessor : public juce::AudioProcessor
    {
    public:
        //==============================================================================
        FloweretAudioProcessor();
        ~FloweretAudioProcessor() override;

        //==============================================================================
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        //==============================================================================
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        //==============================================================================
        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        //==============================================================================
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;

        //==============================================================================
        void getStateInformation (juce::MemoryBlock& destData) override;
        void setStateInformation (const void* data, int sizeInBytes) override;

        // Accessors for UI
        juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts; }
        FloweretSynth& getSynth() noexcept { return synth; }
        juce::AudioFormatManager& getFormatManager() noexcept { return formatManager; }

        bool loadAudioFile (const juce::File& file, bool isLoopOrGranular);
        juce::String getLastLoadedFileName() const noexcept { return lastLoadedFileName; }

    private:
        //==============================================================================
        juce::AudioProcessorValueTreeState apvts;
        FloweretSynth synth;
        juce::AudioFormatManager formatManager;
        juce::String lastLoadedFileName;
        bool lastLoadedWasLoop = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloweretAudioProcessor)
    };
}
