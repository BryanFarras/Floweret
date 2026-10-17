#pragma once

#include <JuceHeader.h>
#include <algorithm>
#include <cmath>

namespace Floweret
{
    class EnvelopeGenerator
    {
    public:
        enum class Stage
        {
            Idle = 0,
            Attack,
            Decay,
            Sustain,
            Release
        };

        EnvelopeGenerator() = default;

        void prepare (double sampleRate)
        {
            currentSampleRate = sampleRate;
            reset();
        }

        void reset()
        {
            stage = Stage::Idle;
            currentLevel = 0.0f;
        }

        void noteOn()
        {
            stage = Stage::Attack;
        }

        void noteOff()
        {
            if (stage != Stage::Idle)
                stage = Stage::Release;
        }

        bool isActive() const noexcept
        {
            return stage != Stage::Idle;
        }

        float getNextSample (float attackSec, float decaySec, float sustainLevel, float releaseSec) noexcept
        {
            const float sr = static_cast<float> (currentSampleRate);

            switch (stage)
            {
                case Stage::Idle:
                    currentLevel = 0.0f;
                    break;

                case Stage::Attack:
                {
                    const float rate = 1.0f / (std::max (0.001f, attackSec) * sr);
                    currentLevel += rate;
                    if (currentLevel >= 1.0f)
                    {
                        currentLevel = 1.0f;
                        stage = Stage::Decay;
                    }
                    break;
                }

                case Stage::Decay:
                {
                    const float rate = (1.0f - sustainLevel) / (std::max (0.001f, decaySec) * sr);
                    currentLevel -= rate;
                    if (currentLevel <= sustainLevel)
                    {
                        currentLevel = sustainLevel;
                        stage = Stage::Sustain;
                    }
                    break;
                }

                case Stage::Sustain:
                    currentLevel = sustainLevel;
                    break;

                case Stage::Release:
                {
                    const float rate = sustainLevel / (std::max (0.001f, releaseSec) * sr);
                    currentLevel -= rate;
                    if (currentLevel <= 0.0f)
                    {
                        currentLevel = 0.0f;
                        stage = Stage::Idle;
                    }
                    break;
                }
            }

            return currentLevel;
        }

        float getCurrentLevel() const noexcept { return currentLevel; }

    private:
        double currentSampleRate = 44100.0;
        Stage stage = Stage::Idle;
        float currentLevel = 0.0f;
    };
}
