#include "MultisampleOscillator.h"

namespace Floweret
{
    MultisampleOscillator::MultisampleOscillator()
    {
        generatePresetBanks();
        reset();
    }

    void MultisampleOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void MultisampleOscillator::reset()
    {
        playhead = 0.0f;
    }

    void MultisampleOscillator::setNoteAndVelocity (int midiNote, float velocity)
    {
        currentNote = midiNote;
        currentVelocity = velocity;
        playhead = 0.0f;

        // Find active zone
        activeZoneIndex = 0;
        for (size_t i = 0; i < zones.size(); ++i)
        {
            if (midiNote >= zones[i].lowKey && midiNote <= zones[i].highKey)
            {
                activeZoneIndex = static_cast<int> (i);
                break;
            }
        }

        // Calculate pitch speed relative to zone root key
        const int root = zones[activeZoneIndex].rootKey;
        const int noteDiff = currentNote - root;
        speed = std::pow (2.0f, static_cast<float> (noteDiff) / 12.0f);
    }

    void MultisampleOscillator::loadPresetBank (int bankIndex)
    {
        generatePresetBanks();
    }

    void MultisampleOscillator::generatePresetBanks()
    {
        // Generate multi-zone acoustic instruments:
        // Zone 0: Bass register (C1 - B2, root C2 = 36)
        // Zone 1: Mid register (C3 - B4, root C4 = 60)
        // Zone 2: Treble register (C5 - G8, root C6 = 84)
        zones.resize (3);

        const int rootNotes[3] = { 36, 60, 84 };
        const int lowKeys[3] = { 0, 48, 72 };
        const int highKeys[3] = { 47, 71, 127 };

        for (int z = 0; z < 3; ++z)
        {
            auto& zone = zones[z];
            zone.lowKey = lowKeys[z];
            zone.highKey = highKeys[z];
            zone.rootKey = rootNotes[z];
            zone.lowVel = 0;
            zone.highVel = 127;

            const int numSamples = static_cast<int> (currentSampleRate * 1.5);
            zone.sampleData.setSize (1, numSamples);
            auto* data = zone.sampleData.getWritePointer (0);

            const float f0 = midiToFreq (static_cast<float> (zone.rootKey));

            for (int i = 0; i < numSamples; ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (currentSampleRate);
                const float env = std::exp (-t * (2.0f + z * 1.5f));

                // Acoustic resonance model
                float s = std::sin (t * MathConstants::twoPi * f0)
                        + 0.5f * std::sin (t * MathConstants::twoPi * f0 * 2.01f) * std::exp (-t * 3.0f)
                        + 0.25f * std::sin (t * MathConstants::twoPi * f0 * 3.02f) * std::exp (-t * 5.0f);

                data[i] = s * env * 0.7f;
            }
        }
    }

    float MultisampleOscillator::getNextSample() noexcept
    {
        if (zones.empty()) return 0.0f;

        const auto& zone = zones[activeZoneIndex];
        const int len = zone.sampleData.getNumSamples();
        if (len <= 0 || playhead >= static_cast<float> (len - 1))
            return 0.0f;

        const auto* data = zone.sampleData.getReadPointer (0);
        const int idx0 = static_cast<int> (playhead);
        const int idx1 = std::min (idx0 + 1, len - 1);
        const float frac = playhead - static_cast<float> (idx0);

        const float sample = linearInterpolate (data[idx0], data[idx1], frac);
        playhead += speed;

        return sample * currentVelocity;
    }
}
