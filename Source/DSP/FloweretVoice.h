#pragma once

#include <JuceHeader.h>
#include "Oscillators/WavetableOscillator.h"
#include "Oscillators/GranularOscillator.h"
#include "Oscillators/SpectralOscillator.h"
#include "Oscillators/SampleOscillator.h"
#include "Oscillators/MultisampleOscillator.h"
#include "Oscillators/NoiseOscillator.h"
#include "Oscillators/SubOscillator.h"
#include "FX/MultiModeFilter.h"
#include "Modulation/EnvelopeGenerator.h"
#include "Modulation/LFO.h"
#include "../Common/Parameters.h"

namespace Floweret
{
    class FloweretVoice
    {
    public:
        FloweretVoice();
        ~FloweretVoice() = default;

        void prepare (double sampleRate);
        void reset();

        void startNote (int midiNoteNumber, float velocity, int currentPitchWheelPosition);
        void stopNote (float velocity, bool allowTailOff);
        void pitchWheelMoved (int newPitchWheelValue);

        bool isActive() const noexcept;
        int getCurrentlyPlayingNote() const noexcept { return currentMidiNote; }

        // Render next audio block
        void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples,
                              const juce::AudioProcessorValueTreeState& apvts,
                              MusicalScale activeScale, int rootKey,
                              juce::AudioBuffer<float>& subDirectBuffer);

    private:
        double currentSampleRate = 44100.0;
        int currentMidiNote = -1;
        float currentVelocity = 0.0f;
        float pitchBendSemitones = 0.0f;

        WavetableOscillator wavetableOsc;
        GranularOscillator granularOsc;
        SpectralOscillator spectralOsc;
        SampleOscillator sampleOsc;
        MultisampleOscillator multisampleOsc;
        NoiseOscillator noiseOsc;
        SubOscillator subOsc;

        MultiModeFilter voiceFilter;
        EnvelopeGenerator ampEnvelope;
        EnvelopeGenerator filterEnvelope;
        LFO voiceLfo;
    };
}
