#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/Parameters.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class EffectsRack
    {
    public:
        EffectsRack();
        ~EffectsRack() = default;

        void prepare (double sampleRate);
        void reset();

        // Modular stage processing
        void processDistortion (juce::AudioBuffer<float>& buffer, DistortionType distType, float distDrive, float distMix);
        void processDelay (juce::AudioBuffer<float>& buffer, float delayTimeSec, float delayFeedback, float delayMix);
        void processReverb (juce::AudioBuffer<float>& buffer, float reverbSize, float reverbDamp, float reverbMix);

        // Process a stereo block through Distortion -> Delay -> Reverb
        void processBlock (juce::AudioBuffer<float>& buffer,
                           DistortionType distType, float distDrive, float distMix,
                           float delayTimeSec, float delayFeedback, float delayMix,
                           float reverbSize, float reverbDamp, float reverbMix);

    private:
        float processDistortionSample (float sample, DistortionType type, float drive) const noexcept;

        double currentSampleRate = 44100.0;

        // Stereo Ping-Pong Delay Line
        std::vector<float> delayBufferL;
        std::vector<float> delayBufferR;
        int delayWritePos = 0;

        // Algorithmic Reverb: 4 comb filters + 2 allpass diffusers per channel
        struct ReverbComb
        {
            std::vector<float> buf;
            int pos = 0;
            float filterState = 0.0f;
        };

        struct ReverbAllPass
        {
            std::vector<float> buf;
            int pos = 0;
            float feedback = 0.5f;
        };

        std::vector<ReverbComb> combsL;
        std::vector<ReverbComb> combsR;
        std::vector<ReverbAllPass> allpassesL;
        std::vector<ReverbAllPass> allpassesR;
    };
}
