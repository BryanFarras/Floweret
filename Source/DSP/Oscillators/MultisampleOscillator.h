#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    struct SampleZone
    {
        int lowKey = 0;
        int highKey = 127;
        int rootKey = 60;
        int lowVel = 0;
        int highVel = 127;
        juce::AudioBuffer<float> sampleData;
    };

    class MultisampleOscillator
    {
    public:
        MultisampleOscillator();
        ~MultisampleOscillator() = default;

        void prepare (double sampleRate);
        void reset();
        void setNoteAndVelocity (int midiNote, float velocity);

        void loadPresetBank (int bankIndex);

        // Process audio
        float getNextSample() noexcept;

    private:
        void generatePresetBanks();

        double currentSampleRate = 44100.0;
        int currentNote = 60;
        float currentVelocity = 1.0f;
        float playhead = 0.0f;
        float speed = 1.0f;
        int activeZoneIndex = 0;

        std::vector<SampleZone> zones;
    };
}
