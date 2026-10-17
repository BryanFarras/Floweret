#include "ResonatorKeyboard.h"

namespace Floweret
{
    ResonatorKeyboard::ResonatorKeyboard (BotanicaLookAndFeel& laf)
        : lookAndFeel (laf)
    {
    }

    bool ResonatorKeyboard::isBlackKey (int midiNote) const
    {
        const int note = midiNote % 12;
        return (note == 1 || note == 3 || note == 6 || note == 8 || note == 10);
    }

    int ResonatorKeyboard::getNoteAtPosition (juce::Point<int> pos) const
    {
        const auto bounds = getLocalBounds().toFloat();
        if (!bounds.contains (pos.toFloat()))
            return -1;

        // Count white keys
        int numWhiteKeys = 0;
        for (int n = startNote; n < startNote + totalNotes; ++n)
        {
            if (!isBlackKey (n))
                numWhiteKeys++;
        }

        const float whiteKeyWidth = bounds.getWidth() / static_cast<float> (numWhiteKeys);
        const float blackKeyHeight = bounds.getHeight() * 0.62f;

        // Check black keys first (since they sit on top)
        if (pos.y < blackKeyHeight)
        {
            int wIdx = 0;
            for (int n = startNote; n < startNote + totalNotes; ++n)
            {
                if (isBlackKey (n))
                {
                    const float blackX = (wIdx * whiteKeyWidth) - (whiteKeyWidth * 0.32f);
                    const float blackW = whiteKeyWidth * 0.64f;
                    if (pos.x >= blackX && pos.x <= (blackX + blackW))
                        return n;
                }
                else
                {
                    wIdx++;
                }
            }
        }

        // Otherwise check white keys
        const int whiteIdx = static_cast<int> (pos.x / whiteKeyWidth);
        int currentWhite = 0;
        for (int n = startNote; n < startNote + totalNotes; ++n)
        {
            if (!isBlackKey (n))
            {
                if (currentWhite == whiteIdx)
                    return n;
                currentWhite++;
            }
        }

        return -1;
    }

    void ResonatorKeyboard::paint (juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float h = bounds.getHeight();

        // Count white keys
        int numWhiteKeys = 0;
        for (int n = startNote; n < startNote + totalNotes; ++n)
        {
            if (!isBlackKey (n))
                numWhiteKeys++;
        }

        const float whiteKeyWidth = bounds.getWidth() / static_cast<float> (numWhiteKeys);
        const float blackKeyHeight = h * 0.62f;

        // 1. Draw White Keys (Ivory cream)
        int whiteIndex = 0;
        for (int n = startNote; n < startNote + totalNotes; ++n)
        {
            if (!isBlackKey (n))
            {
                const float x = whiteIndex * whiteKeyWidth;
                auto keyRect = juce::Rectangle<float> (x, 0.0f, whiteKeyWidth, h);

                juce::Colour keyCol = (n == activeNote)
                    ? lookAndFeel.getPeachCoral()
                    : lookAndFeel.getWhisperWhite();

                g.setColour (keyCol);
                g.fillRect (keyRect);

                // Right separator line
                g.setColour (lookAndFeel.getPlatinumBorder());
                g.drawVerticalLine (static_cast<int> (x + whiteKeyWidth), 0.0f, h);

                // Octave labels (C3, C4, C5, C6, C7)
                if (n % 12 == 0)
                {
                    const int oct = (n / 12) - 1;
                    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
                    g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.7f));
                    g.drawText ("C" + juce::String (oct), keyRect.withTrimmedBottom (4.0f),
                                juce::Justification::bottomLeft, true);
                }

                whiteIndex++;
            }
        }

        // 2. Draw Black Keys (Dark Espresso / Warm Charcoal with soft peachy active highlight)
        whiteIndex = 0;
        for (int n = startNote; n < startNote + totalNotes; ++n)
        {
            if (isBlackKey (n))
            {
                const float blackX = (whiteIndex * whiteKeyWidth) - (whiteKeyWidth * 0.32f);
                const float blackW = whiteKeyWidth * 0.64f;
                auto blackRect = juce::Rectangle<float> (blackX, 0.0f, blackW, blackKeyHeight);

                juce::Colour blackCol = (n == activeNote)
                    ? lookAndFeel.getGoldenPeach()
                    : lookAndFeel.getDarkEspresso();

                g.setColour (blackCol);
                g.fillRoundedRectangle (blackRect, 2.5f);

                // Subtle inner top highlight
                g.setColour (juce::Colour::fromRGBA (255, 255, 255, 25));
                g.drawRoundedRectangle (blackRect.reduced (0.5f), 2.5f, 1.0f);
            }
            else
            {
                whiteIndex++;
            }
        }

        // Outer Keyboard Border
        g.setColour (lookAndFeel.getPlatinumBorder());
        g.drawRect (bounds, 1.0f);
    }

    void ResonatorKeyboard::mouseDown (const juce::MouseEvent& e)
    {
        const int note = getNoteAtPosition (e.getPosition());
        if (note >= 0)
        {
            activeNote = note;
            if (onNoteOn)
                onNoteOn (note, 0.85f);
            repaint();
        }
    }

    void ResonatorKeyboard::mouseDrag (const juce::MouseEvent& e)
    {
        const int note = getNoteAtPosition (e.getPosition());
        if (note != activeNote)
        {
            if (activeNote >= 0 && onNoteOff)
                onNoteOff (activeNote, 0.0f);

            activeNote = note;
            if (activeNote >= 0 && onNoteOn)
                onNoteOn (activeNote, 0.85f);

            repaint();
        }
    }

    void ResonatorKeyboard::mouseUp (const juce::MouseEvent& e)
    {
        if (activeNote >= 0)
        {
            if (onNoteOff)
                onNoteOff (activeNote, 0.0f);
            activeNote = -1;
            repaint();
        }
    }
}
