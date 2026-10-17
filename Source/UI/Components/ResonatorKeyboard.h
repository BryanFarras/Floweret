#pragma once

#include <JuceHeader.h>
#include <functional>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    class ResonatorKeyboard : public juce::Component
    {
    public:
        static constexpr int startNote = 48; // C3
        static constexpr int totalNotes = 49; // C3 to C7

        ResonatorKeyboard (BotanicaLookAndFeel& laf);
        ~ResonatorKeyboard() override = default;

        void paint (juce::Graphics& g) override;
        void mouseDown (const juce::MouseEvent& e) override;
        void mouseDrag (const juce::MouseEvent& e) override;
        void mouseUp (const juce::MouseEvent& e) override;

        std::function<void(int, float)> onNoteOn;
        std::function<void(int, float)> onNoteOff;

    private:
        int getNoteAtPosition (juce::Point<int> pos) const;
        bool isBlackKey (int midiNote) const;

        BotanicaLookAndFeel& lookAndFeel;
        int activeNote = -1;
    };
}
