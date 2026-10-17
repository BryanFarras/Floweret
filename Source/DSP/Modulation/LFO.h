#pragma once

#include <JuceHeader.h>
#include <cmath>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class LFO
    {
    public:
        LFO() : rng() {}

        void prepare (double sampleRate)
        {
            currentSampleRate = sampleRate;
            reset();
        }

        void reset()
        {
            phase = 0.0f;
            lastSampleAndHold = 0.0f;
            chaosDrift = 0.0f;
        }

        float getNextSample (float rateHz, int shape, float chaos) noexcept
        {
            const float phaseInc = rateHz / static_cast<float> (currentSampleRate);
            phase += phaseInc;
            if (phase >= 1.0f)
            {
                phase -= 1.0f;
                lastSampleAndHold = rng.nextFloat();
            }

            float out = 0.0f;
            switch (shape)
            {
                case 0: // Sine
                    out = std::sin (phase * MathConstants::twoPi);
                    break;
                case 1: // Triangle
                    out = 4.0f * std::abs (phase - 0.5f) - 1.0f;
                    break;
                case 2: // Saw Down
                    out = 1.0f - 2.0f * phase;
                    break;
                case 3: // Saw Up
                    out = 2.0f * phase - 1.0f;
                    break;
                case 4: // Square
                    out = (phase < 0.5f) ? 1.0f : -1.0f;
                    break;
                case 5: // Random S&H
                    out = lastSampleAndHold;
                    break;
                default:
                    out = std::sin (phase * MathConstants::twoPi);
                    break;
            }

            // Smooth chaos drift
            if (chaos > 0.001f)
            {
                chaosDrift = 0.995f * chaosDrift + 0.005f * rng.nextFloat();
                out = linearInterpolate (out, chaosDrift, chaos * 0.7f);
            }

            return out;
        }

    private:
        double currentSampleRate = 44100.0;
        float phase = 0.0f;
        float lastSampleAndHold = 0.0f;
        float chaosDrift = 0.0f;
        FastRandom rng;
    };
}
