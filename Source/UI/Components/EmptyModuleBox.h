#pragma once

#include <JuceHeader.h>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    class EmptyModuleBox : public juce::Component
    {
    public:
        EmptyModuleBox (BotanicaLookAndFeel& laf, const juce::String& label = "")
            : lookAndFeel (laf), boxLabel (label)
        {
        }

        void setBoxLabel (const juce::String& newLabel)
        {
            boxLabel = newLabel;
            repaint();
        }

        void paint (juce::Graphics& g) override
        {
            const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

            // Frosted Soft Peachy Card background (#F8EDEB)
            g.setColour (lookAndFeel.getWhisperWhite());
            g.fillRoundedRectangle (bounds, 5.0f);

            // Platinum border
            g.setColour (lookAndFeel.getPlatinumBorder());
            g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

            // Subtle dashed inner placeholder outline
            auto innerBounds = bounds.reduced (6.0f);
            g.setColour (lookAndFeel.getPlatinumBorder().withAlpha (0.6f));
            float dashLengths[] = { 4.0f, 4.0f };
            g.drawDashedLine (juce::Line<float> (innerBounds.getX(), innerBounds.getY(), innerBounds.getRight(), innerBounds.getY()), dashLengths, 2, 1.0f);
            g.drawDashedLine (juce::Line<float> (innerBounds.getRight(), innerBounds.getY(), innerBounds.getRight(), innerBounds.getBottom()), dashLengths, 2, 1.0f);
            g.drawDashedLine (juce::Line<float> (innerBounds.getRight(), innerBounds.getBottom(), innerBounds.getX(), innerBounds.getBottom()), dashLengths, 2, 1.0f);
            g.drawDashedLine (juce::Line<float> (innerBounds.getX(), innerBounds.getBottom(), innerBounds.getX(), innerBounds.getY()), dashLengths, 2, 1.0f);

            // Centered empty box watermark text
            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (lookAndFeel.getMutedSlate().withAlpha (0.4f));
            g.drawText (boxLabel.isNotEmpty() ? boxLabel : "[ EMPTY SLOT ]", bounds, juce::Justification::centred, true);
        }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        juce::String boxLabel;
    };
}
