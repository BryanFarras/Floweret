#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/Parameters.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class MultiModeFilter
    {
    public:
        MultiModeFilter();
        ~MultiModeFilter() = default;

        void prepare (double sampleRate);
        void reset();

        float processSample (float input, FilterType type, float cutoffHz, float resonance, float drive) noexcept;

    private:
        double currentSampleRate = 44100.0;

        // State Variable Filter (SVF) internal states
        float s1 = 0.0f, s2 = 0.0f;
        float s3 = 0.0f, s4 = 0.0f; // 2nd stage for 24dB

        // Comb filter buffer
        std::vector<float> combBuffer;
        int combWriteIndex = 0;

        // Formant filter states
        float f1_s1 = 0.0f, f1_s2 = 0.0f;
        float f2_s1 = 0.0f, f2_s2 = 0.0f;
    };
}
