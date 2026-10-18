#include "EffectsRack.h"

namespace Floweret
{
    EffectsRack::EffectsRack()
    {
        delayBufferL.resize (88200, 0.0f);
        delayBufferR.resize (88200, 0.0f);
        reset();
    }

    void EffectsRack::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        const int maxDelaySamples = static_cast<int> (sampleRate * 2.5);
        delayBufferL.assign (maxDelaySamples, 0.0f);
        delayBufferR.assign (maxDelaySamples, 0.0f);

        // Schroeder/Moorer algorithmic reverb delay lengths scaled by sample rate
        const int combTuningsL[4] = { 1116, 1188, 1277, 1356 };
        const int combTuningsR[4] = { 1139, 1211, 1294, 1379 };
        const int allpassTunings[2] = { 225, 556 };

        const float srScale = static_cast<float> (sampleRate) / 44100.0f;

        combsL.resize (4);
        combsR.resize (4);
        for (int i = 0; i < 4; ++i)
        {
            combsL[i].buf.assign (static_cast<size_t> (combTuningsL[i] * srScale), 0.0f);
            combsL[i].pos = 0;
            combsL[i].filterState = 0.0f;

            combsR[i].buf.assign (static_cast<size_t> (combTuningsR[i] * srScale), 0.0f);
            combsR[i].pos = 0;
            combsR[i].filterState = 0.0f;
        }

        allpassesL.resize (2);
        allpassesR.resize (2);
        for (int i = 0; i < 2; ++i)
        {
            allpassesL[i].buf.assign (static_cast<size_t> (allpassTunings[i] * srScale), 0.0f);
            allpassesL[i].pos = 0;
            allpassesL[i].feedback = 0.5f;

            allpassesR[i].buf.assign (static_cast<size_t> ((allpassTunings[i] + 23) * srScale), 0.0f);
            allpassesR[i].pos = 0;
            allpassesR[i].feedback = 0.5f;
        }

        reset();
    }

    void EffectsRack::reset()
    {
        std::fill (delayBufferL.begin(), delayBufferL.end(), 0.0f);
        std::fill (delayBufferR.begin(), delayBufferR.end(), 0.0f);
        delayWritePos = 0;

        for (auto& c : combsL) { std::fill (c.buf.begin(), c.buf.end(), 0.0f); c.pos = 0; c.filterState = 0.0f; }
        for (auto& c : combsR) { std::fill (c.buf.begin(), c.buf.end(), 0.0f); c.pos = 0; c.filterState = 0.0f; }
        for (auto& a : allpassesL) { std::fill (a.buf.begin(), a.buf.end(), 0.0f); a.pos = 0; }
        for (auto& a : allpassesR) { std::fill (a.buf.begin(), a.buf.end(), 0.0f); a.pos = 0; }
    }

    float EffectsRack::processDistortionSample (float sample, DistortionType type, float drive) const noexcept
    {
        if (drive < 0.001f) return sample;

        const float gain = 1.0f + drive * 9.0f;
        float x = sample * gain;

        switch (type)
        {
            case DistortionType::SoftClip:
                return fastTanh (x) / std::sqrt (gain);

            case DistortionType::HardClip:
                return std::max (-1.0f, std::min (x, 1.0f)) * 0.75f;

            case DistortionType::Foldback:
            {
                // Glitch wavefolder: folds signal when exceeding threshold
                while (x > 1.0f || x < -1.0f)
                {
                    if (x > 1.0f) x = 2.0f - x;
                    if (x < -1.0f) x = -2.0f - x;
                }
                return x * 0.8f;
            }

            case DistortionType::Bitcrush:
            {
                // Bit reduction / decimation
                const float steps = std::max (4.0f, 64.0f * (1.0f - drive * 0.9f));
                return std::round (x * steps) / steps;
            }

            case DistortionType::TubeDrive:
            {
                // Asymmetric tube distortion (adds even harmonics)
                if (x > 0.0f)
                    return (1.0f - std::exp (-x)) * 0.85f;
                else
                    return -fastTanh (-x) * 0.85f;
            }

            default:
                return sample;
        }
    }

    void EffectsRack::processDistortion (juce::AudioBuffer<float>& buffer, DistortionType distType, float distDrive, float distMix)
    {
        if (distMix <= 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        auto* left = buffer.getWritePointer (0);
        auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : left;

        for (int i = 0; i < numSamples; ++i)
        {
            const float distL = processDistortionSample (left[i], distType, distDrive);
            const float distR = processDistortionSample (right[i], distType, distDrive);
            left[i] = linearInterpolate (left[i], distL, distMix);
            right[i] = linearInterpolate (right[i], distR, distMix);
        }
    }

    void EffectsRack::processDelay (juce::AudioBuffer<float>& buffer, float delayTimeSec, float delayFeedback, float delayMix)
    {
        if (delayMix <= 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        auto* left = buffer.getWritePointer (0);
        auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : left;

        const int delayLengthSamples = std::max (1, std::min (static_cast<int> (delayTimeSec * currentSampleRate),
                                                              static_cast<int> (delayBufferL.size() - 4)));
        const int maxDelayBuf = static_cast<int> (delayBufferL.size());

        for (int i = 0; i < numSamples; ++i)
        {
            int readL = delayWritePos - delayLengthSamples;
            if (readL < 0) readL += maxDelayBuf;

            int readR = delayWritePos - (delayLengthSamples / 2);
            if (readR < 0) readR += maxDelayBuf;

            const float delayedL = delayBufferL[static_cast<size_t> (readL)];
            const float delayedR = delayBufferR[static_cast<size_t> (readR)];

            delayBufferL[static_cast<size_t> (delayWritePos)] = left[i] + delayedR * delayFeedback;
            delayBufferR[static_cast<size_t> (delayWritePos)] = right[i] + delayedL * delayFeedback;

            left[i] = linearInterpolate (left[i], delayedL, delayMix);
            right[i] = linearInterpolate (right[i], delayedR, delayMix);

            delayWritePos = (delayWritePos + 1) % maxDelayBuf;
        }
    }

    void EffectsRack::processReverb (juce::AudioBuffer<float>& buffer, float reverbSize, float reverbDamp, float reverbMix)
    {
        if (reverbMix <= 0.001f)
            return;

        const int numSamples = buffer.getNumSamples();
        auto* left = buffer.getWritePointer (0);
        auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : left;

        const float feedback = 0.7f + reverbSize * 0.28f;
        const float dampCoeff = 0.1f + reverbDamp * 0.85f;

        for (int i = 0; i < numSamples; ++i)
        {
            const float inputMono = (left[i] + right[i]) * 0.5f;

            float combSumL = 0.0f;
            float combSumR = 0.0f;

            for (auto& c : combsL)
            {
                const float delayed = c.buf[static_cast<size_t> (c.pos)];
                c.filterState = delayed * (1.0f - dampCoeff) + c.filterState * dampCoeff;
                c.buf[static_cast<size_t> (c.pos)] = inputMono + c.filterState * feedback;
                c.pos = (c.pos + 1) % static_cast<int> (c.buf.size());
                combSumL += delayed;
            }

            for (auto& c : combsR)
            {
                const float delayed = c.buf[static_cast<size_t> (c.pos)];
                c.filterState = delayed * (1.0f - dampCoeff) + c.filterState * dampCoeff;
                c.buf[static_cast<size_t> (c.pos)] = inputMono + c.filterState * feedback;
                c.pos = (c.pos + 1) % static_cast<int> (c.buf.size());
                combSumR += delayed;
            }

            float diffL = combSumL * 0.25f;
            for (auto& a : allpassesL)
            {
                const float bufOut = a.buf[static_cast<size_t> (a.pos)];
                const float temp = diffL + bufOut * a.feedback;
                a.buf[static_cast<size_t> (a.pos)] = temp;
                diffL = -diffL + temp * (1.0f - a.feedback);
                a.pos = (a.pos + 1) % static_cast<int> (a.buf.size());
            }

            float diffR = combSumR * 0.25f;
            for (auto& a : allpassesR)
            {
                const float bufOut = a.buf[static_cast<size_t> (a.pos)];
                const float temp = diffR + bufOut * a.feedback;
                a.buf[static_cast<size_t> (a.pos)] = temp;
                diffR = -diffR + temp * (1.0f - a.feedback);
                a.pos = (a.pos + 1) % static_cast<int> (a.buf.size());
            }

            left[i] = linearInterpolate (left[i], diffL, reverbMix);
            right[i] = linearInterpolate (right[i], diffR, reverbMix);
        }
    }

    void EffectsRack::processBlock (juce::AudioBuffer<float>& buffer,
                                   DistortionType distType, float distDrive, float distMix,
                                   float delayTimeSec, float delayFeedback, float delayMix,
                                   float reverbSize, float reverbDamp, float reverbMix)
    {
        processDistortion (buffer, distType, distDrive, distMix);
        processDelay (buffer, delayTimeSec, delayFeedback, delayMix);
        processReverb (buffer, reverbSize, reverbDamp, reverbMix);
    }
}
