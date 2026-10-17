#pragma once

#include <JuceHeader.h>
#include <vector>
#include <algorithm>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class Arpeggiator
    {
    public:
        Arpeggiator() = default;

        void prepare (double sampleRate)
        {
            currentSampleRate = sampleRate;
            reset();
        }

        void reset()
        {
            currentStep = 0;
            sampleCounter = 0;
            notes.clear();
        }

        void handleNoteOn (int midiNote)
        {
            if (std::find (notes.begin(), notes.end(), midiNote) == notes.end())
            {
                notes.push_back (midiNote);
                std::sort (notes.begin(), notes.end());
            }
        }

        void handleNoteOff (int midiNote)
        {
            notes.erase (std::remove (notes.begin(), notes.end(), midiNote), notes.end());
        }

        bool hasNotes() const noexcept { return !notes.empty(); }

        // Advance sample clock and check if a new arp note should trigger
        // Returns MIDI note number, or -1 if no trigger on this sample
        int processSampleClock (bool active, int mode, int rateIndex, int octaveRange, double bpm) noexcept
        {
            if (!active || notes.empty())
                return -1;

            // Arp step length in samples based on BPM & rate index
            // 0: 1/4, 1: 1/8, 2: 1/16, 3: 1/32, 4: 1/8T, 5: 1/16T
            double division = 4.0;
            if (rateIndex == 0) division = 1.0;
            else if (rateIndex == 1) division = 2.0;
            else if (rateIndex == 2) division = 4.0;
            else if (rateIndex == 3) division = 8.0;
            else if (rateIndex == 4) division = 3.0; // 1/8 triplet
            else if (rateIndex == 5) division = 6.0; // 1/16 triplet

            const double samplesPerBeat = (currentSampleRate * 60.0) / std::max (30.0, bpm);
            const int stepInterval = std::max (64, static_cast<int> (samplesPerBeat / division));

            sampleCounter++;
            if (sampleCounter >= stepInterval)
            {
                sampleCounter = 0;

                // Build note pool across octave range
                std::vector<int> arpPool;
                for (int oct = 0; oct < octaveRange; ++oct)
                {
                    for (int n : notes)
                    {
                        const int octNote = n + (oct * 12);
                        if (octNote <= 127)
                            arpPool.push_back (octNote);
                    }
                }

                if (arpPool.empty()) return -1;

                int triggeredNote = -1;
                switch (mode)
                {
                    case 0: // Up
                        triggeredNote = arpPool[static_cast<size_t> (currentStep % arpPool.size())];
                        currentStep++;
                        break;

                    case 1: // Down
                        triggeredNote = arpPool[arpPool.size() - 1 - static_cast<size_t> (currentStep % arpPool.size())];
                        currentStep++;
                        break;

                    case 2: // Up/Down
                    {
                        const int period = std::max (1, static_cast<int> (arpPool.size() * 2 - 2));
                        const int modStep = currentStep % period;
                        const int idx = (modStep < static_cast<int> (arpPool.size()))
                                      ? modStep
                                      : (period - modStep);
                        triggeredNote = arpPool[static_cast<size_t> (idx)];
                        currentStep++;
                        break;
                    }

                    case 3: // Random
                    {
                        const int rndIdx = static_cast<int> (rng.nextUnsignedFloat() * static_cast<float> (arpPool.size())) % arpPool.size();
                        triggeredNote = arpPool[static_cast<size_t> (rndIdx)];
                        break;
                    }

                    case 4: // Chord
                        triggeredNote = arpPool[0]; // Triggers fundamental
                        break;
                }

                return triggeredNote;
            }

            return -1;
        }

    private:
        double currentSampleRate = 44100.0;
        int currentStep = 0;
        int sampleCounter = 0;
        std::vector<int> notes;
        FastRandom rng;
    };
}
