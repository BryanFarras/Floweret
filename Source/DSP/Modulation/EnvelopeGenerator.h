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
            releaseStartLevel = 1.0f;
        }

        void noteOn()
        {
            stage = Stage::Attack;
            if (currentLevel < 0.0f || currentLevel > 1.0f)
                currentLevel = 0.0f;
        }

        void noteOff()
        {
            if (stage != Stage::Idle)
            {
                releaseStartLevel = currentLevel;
                if (releaseStartLevel <= 0.0001f)
                {
                    currentLevel = 0.0f;
                    stage = Stage::Idle;
                }
                else
                {
                    stage = Stage::Release;
                }
            }
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
                    const float aTime = std::max (0.001f, attackSec);
                    const float rate = 1.0f / (aTime * sr);
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
                    const float targetSustain = std::max (0.0f, std::min (1.0f, sustainLevel));
                    const float decayRange = std::max (0.0001f, 1.0f - targetSustain);
                    const float dTime = std::max (0.001f, decaySec);
                    const float rate = decayRange / (dTime * sr);
                    currentLevel -= rate;
                    if (currentLevel <= targetSustain)
                    {
                        currentLevel = targetSustain;
                        if (targetSustain <= 0.0001f)
                        {
                            currentLevel = 0.0f;
                            stage = Stage::Idle;
                        }
                        else
                        {
                            stage = Stage::Sustain;
                        }
                    }
                    break;
                }

                case Stage::Sustain:
                {
                    const float targetSustain = std::max (0.0f, std::min (1.0f, sustainLevel));
                    currentLevel = targetSustain;
                    if (targetSustain <= 0.0001f)
                    {
                        currentLevel = 0.0f;
                        stage = Stage::Idle;
                    }
                    break;
                }

                case Stage::Release:
                {
                    const float rTime = std::max (0.001f, releaseSec);
                    const float base = (releaseStartLevel > 0.0001f) ? releaseStartLevel : 1.0f;
                    const float rate = base / (rTime * sr);
                    currentLevel -= rate;
                    if (currentLevel <= 0.0001f)
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
        float releaseStartLevel = 1.0f;
    };
}
