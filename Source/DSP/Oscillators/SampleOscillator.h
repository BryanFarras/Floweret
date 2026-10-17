#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class SampleOscillator
    {
    public:
        SampleOscillator();
        ~SampleOscillator() = default;

        void prepare (double sampleRate);
        void reset();
        void setFrequency (float freq);

        void loadSample (const juce::AudioBuffer<float>& newBuffer);
        void generateFactoryAcousticPluck();

        // Detect zero crossings and snap loop points
        void snapLoopPointsToZeroCrossings();

        // Process audio
        float getNextSample (int loopMode, float tapeStopAmount, int sliceIndex) noexcept;

    private:
        double currentSampleRate = 44100.0;
        float currentFrequency = 261.63f;

        juce::AudioBuffer<float> buffer;
        float playhead = 0.0f;
        float playbackSpeed = 1.0f;
        float currentRate = 1.0f;
        bool pingPongReverse = false;

        int loopStart = 0;
        int loopEnd = 0;
        std::vector<int> sliceMarkers;
    };
}
