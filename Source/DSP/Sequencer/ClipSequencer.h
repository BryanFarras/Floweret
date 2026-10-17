#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    struct SequenceStep
    {
        bool active = true;
        int semitoneOffset = 0; // -12 to +12
        float velocity = 0.8f;
        float gate = 0.75f;
    };

    class ClipSequencer
    {
    public:
        static constexpr int numSteps = 16;

        ClipSequencer()
        {
            steps.resize (numSteps);
            // Default melodic pattern for instant inspiration
            const int defaultOffsets[numSteps] = { 0, 0, 3, 7, 0, 5, 7, 10, 0, 2, 3, 5, 7, 10, 12, 7 };
            for (int i = 0; i < numSteps; ++i)
            {
                steps[i].active = (i % 2 == 0) || (i == 7) || (i == 15);
                steps[i].semitoneOffset = defaultOffsets[i];
                steps[i].velocity = 0.85f;
                steps[i].gate = 0.8f;
            }
        }

        void prepare (double sampleRate)
        {
            currentSampleRate = sampleRate;
            reset();
        }

        void reset()
        {
            currentStep = 0;
            sampleTimer = 0;
        }

        std::vector<SequenceStep>& getSteps() { return steps; }
        const std::vector<SequenceStep>& getSteps() const { return steps; }
        int getCurrentStep() const noexcept { return currentStep; }

        // Process step timing
        // Returns true if new step triggered, populating outSemitone and outVel
        bool processClock (double bpm, float glitchProb, int& outSemitone, float& outVel) noexcept
        {
            const double samplesPerBeat = (currentSampleRate * 60.0) / std::max (30.0, bpm);
            // 1/16th note steps
            const int stepInterval = std::max (64, static_cast<int> (samplesPerBeat / 4.0));

            sampleTimer++;
            if (sampleTimer >= stepInterval)
            {
                sampleTimer = 0;
                currentStep = (currentStep + 1) % numSteps;

                auto& step = steps[static_cast<size_t> (currentStep)];

                // Glitch probability: random ratchet or octave jump
                if (glitchProb > 0.01f && rng.nextUnsignedFloat() < glitchProb)
                {
                    outSemitone = step.semitoneOffset + ((rng.nextFloat() > 0.0f) ? 12 : -12);
                    outVel = std::min (1.0f, step.velocity * 1.2f);
                    return true;
                }

                if (step.active)
                {
                    outSemitone = step.semitoneOffset;
                    outVel = step.velocity;
                    return true;
                }
            }

            return false;
        }

    private:
        double currentSampleRate = 44100.0;
        int currentStep = 0;
        int sampleTimer = 0;
        std::vector<SequenceStep> steps;
        FastRandom rng;
    };
}
