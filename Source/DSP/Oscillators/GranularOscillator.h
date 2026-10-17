#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    struct Grain
    {
        bool active = false;
        float bufferPosition = 0.0f; // Current playhead inside audio buffer (in samples)
        float speed = 1.0f;          // Playback speed (pitch ratio)
        int lengthSamples = 0;       // Total grain duration
        int currentSample = 0;       // Playback age
        float panLeft = 0.707f;      // Stereo pan
        float panRight = 0.707f;
    };

    class GranularOscillator
    {
    public:
        static constexpr int maxGrains = 64;

        GranularOscillator();
        ~GranularOscillator() = default;

        void prepare (double sampleRate);
        void reset();
        void setFrequency (float freq);

        // Load custom sample into granular memory or generate internal botanical soundscape
        void loadSample (const juce::AudioBuffer<float>& newBuffer);
        void generateBotanicaSample();

        // Process audio and return stereo sample pair
        std::pair<float, float> getNextSample (float positionNorm, float grainSizeMs,
                                              float density, float spray, float pitchJitterSemitones,
                                              bool freeze) noexcept;

        // Provide grain states for UI particle visualization
        const std::vector<Grain>& getActiveGrains() const { return grains; }
        float getBufferLengthSeconds() const;

    private:
        void spawnGrain (float positionNorm, float grainSizeMs, float spray, float pitchJitterSemitones);

        double currentSampleRate = 44100.0;
        float currentFrequency = 261.63f; // Middle C
        FastRandom rng;

        juce::AudioBuffer<float> sampleBuffer;
        std::vector<Grain> grains;

        float grainSpawnAccumulator = 0.0f;
    };
}
