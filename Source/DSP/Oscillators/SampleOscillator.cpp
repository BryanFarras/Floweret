#include "SampleOscillator.h"

namespace Floweret
{
    SampleOscillator::SampleOscillator()
    {
        generateFactoryAcousticPluck();
        reset();
    }

    void SampleOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void SampleOscillator::reset()
    {
        playhead = 0.0f;
        currentRate = 1.0f;
        pingPongReverse = false;
    }

    void SampleOscillator::setFrequency (float freq)
    {
        currentFrequency = std::max (10.0f, std::min (freq, 20000.0f));
        playbackSpeed = currentFrequency / 261.63f; // relative to C4
    }

    void SampleOscillator::generateFactoryAcousticPluck()
    {
        // Generate a 2-second organic plucked string sound
        const int numSamples = static_cast<int> (currentSampleRate * 2.0);
        buffer.setSize (1, numSamples);
        auto* data = buffer.getWritePointer (0);

        for (int i = 0; i < numSamples; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (currentSampleRate);
            const float decay = std::exp (-t * 3.5f);
            const float f0 = 261.63f;

            // Pluck harmonics with non-linear strike
            float val = std::sin (t * MathConstants::twoPi * f0)
                      + 0.6f * std::sin (t * MathConstants::twoPi * f0 * 2.0f) * std::exp (-t * 6.0f)
                      + 0.3f * std::sin (t * MathConstants::twoPi * f0 * 3.0f) * std::exp (-t * 9.0f)
                      + 0.15f * std::sin (t * MathConstants::twoPi * f0 * 4.0f) * std::exp (-t * 12.0f);

            data[i] = val * decay;
        }

        loopStart = static_cast<int> (currentSampleRate * 0.1);
        loopEnd = static_cast<int> (currentSampleRate * 1.5);
        snapLoopPointsToZeroCrossings();

        // 8 slice markers
        sliceMarkers.clear();
        for (int s = 0; s < 8; ++s)
            sliceMarkers.push_back (static_cast<int> (s * (numSamples / 8)));
    }

    void SampleOscillator::loadSample (const juce::AudioBuffer<float>& newBuffer)
    {
        if (newBuffer.getNumSamples() > 0)
        {
            buffer.makeCopyOf (newBuffer);
            loopStart = 0;
            loopEnd = buffer.getNumSamples() - 1;
            snapLoopPointsToZeroCrossings();
        }
    }

    void SampleOscillator::snapLoopPointsToZeroCrossings()
    {
        const int len = buffer.getNumSamples();
        if (len < 100) return;

        const auto* data = buffer.getReadPointer (0);

        // Snap loop start to nearest positive-going zero crossing
        int bestStart = loopStart;
        for (int i = std::max (1, loopStart - 200); i < std::min (len - 1, loopStart + 200); ++i)
        {
            if (data[i - 1] <= 0.0f && data[i] > 0.0f)
            {
                bestStart = i;
                break;
            }
        }
        loopStart = bestStart;

        // Snap loop end
        int bestEnd = loopEnd;
        for (int i = std::max (1, loopEnd - 200); i < std::min (len - 1, loopEnd + 200); ++i)
        {
            if (data[i - 1] <= 0.0f && data[i] > 0.0f)
            {
                bestEnd = i;
                break;
            }
        }
        loopEnd = bestEnd;
    }

    float SampleOscillator::getNextSample (int loopMode, float tapeStopAmount, int sliceIndex) noexcept
    {
        const int len = buffer.getNumSamples();
        if (len <= 0) return 0.0f;

        // Tape-stop rate deceleration effect
        if (tapeStopAmount > 0.01f)
        {
            const float targetRate = std::max (0.0f, 1.0f - tapeStopAmount);
            currentRate = linearInterpolate (currentRate, targetRate, 0.02f);
        }
        else
        {
            currentRate = linearInterpolate (currentRate, 1.0f, 0.05f);
        }

        const float step = playbackSpeed * currentRate;
        const auto* data = buffer.getReadPointer (0);

        // Read sample with interpolation
        const int idx0 = static_cast<int> (playhead);
        const int idx1 = std::min (idx0 + 1, len - 1);
        const float frac = playhead - static_cast<float> (idx0);
        const float sample = linearInterpolate (data[idx0], data[idx1], frac);

        // Advance playhead according to loop mode
        if (loopMode == 0) // One-shot
        {
            playhead += step;
            if (playhead >= static_cast<float> (len - 1))
                playhead = static_cast<float> (len - 1);
        }
        else if (loopMode == 1) // Forward Loop
        {
            playhead += step;
            if (playhead >= static_cast<float> (loopEnd))
                playhead = static_cast<float> (loopStart);
        }
        else // Ping-pong Loop
        {
            if (pingPongReverse)
            {
                playhead -= step;
                if (playhead <= static_cast<float> (loopStart))
                {
                    playhead = static_cast<float> (loopStart);
                    pingPongReverse = false;
                }
            }
            else
            {
                playhead += step;
                if (playhead >= static_cast<float> (loopEnd))
                {
                    playhead = static_cast<float> (loopEnd);
                    pingPongReverse = true;
                }
            }
        }

        return sample;
    }
}
