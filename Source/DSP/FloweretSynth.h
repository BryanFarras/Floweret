#pragma once

#include <JuceHeader.h>
#include <vector>
#include "FloweretVoice.h"
#include "FX/ColorResonator.h"
#include "FX/EffectsRack.h"
#include "Sequencer/Arpeggiator.h"
#include "Sequencer/ClipSequencer.h"
#include "../Common/Parameters.h"

namespace Floweret
{
    class FloweretSynth
    {
    public:
        static constexpr int maxVoices = 16;

        FloweretSynth();
        ~FloweretSynth() = default;

        void prepare (double sampleRate, int samplesPerBlock);
        void reset();

        // Process audio block and MIDI messages
        void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                           const juce::AudioProcessorValueTreeState& apvts, double bpm = 120.0);

        void handleNoteOn (int midiNote, float velocity);
        void handleNoteOff (int midiNote, float velocity);
        void handlePitchWheel (int pitchWheelValue);
        void allNotesOff();

        int getActiveVoiceCount() const noexcept;
        const ColorResonator& getColorResonator() const { return colorResonator; }

        void loadSample (const juce::AudioBuffer<float>& sample, bool isGranular);

        void setEffectChain (const std::vector<int>& chain)
        {
            juce::ScopedLock sl (synthLock);
            effectChain = chain;
        }

        std::vector<int> getEffectChain() const
        {
            juce::ScopedLock sl (synthLock);
            return effectChain;
        }

    private:
        double currentSampleRate = 44100.0;
        int currentPitchWheel = 8192;
        juce::CriticalSection synthLock;

        std::vector<FloweretVoice> voices;
        ColorResonator colorResonator;
        EffectsRack effectsRack;
        Arpeggiator arpeggiator;
        ClipSequencer clipSequencer;

        std::vector<int> effectChain = { 0, 1, 2, 3 };

        juce::AudioBuffer<float> subDirectBuffer;

        FloweretVoice* findFreeVoice();
    };
}
