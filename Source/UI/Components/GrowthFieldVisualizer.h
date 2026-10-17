#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../BotanicaLookAndFeel.h"

namespace Floweret
{
    struct FloatingCard
    {
        float xNorm = 0.0f;
        float yNorm = 0.0f;
        float widthNorm = 0.0f;
        float heightNorm = 0.0f;
        float opacity = 0.2f;
        float speed = 0.002f;
        float phase = 0.0f;
    };

    class GrowthFieldVisualizer : public juce::Component, public juce::Timer
    {
    public:
        GrowthFieldVisualizer (BotanicaLookAndFeel& laf);
        ~GrowthFieldVisualizer() override;

        void paint (juce::Graphics& g) override;
        void timerCallback() override;

        void setAudioActivity (float activity) { currentActivity = 0.8f * currentActivity + 0.2f * activity; }

    private:
        BotanicaLookAndFeel& lookAndFeel;
        std::vector<FloatingCard> cards;
        float animationPhase = 0.0f;
        float currentActivity = 0.0f;
    };
}
