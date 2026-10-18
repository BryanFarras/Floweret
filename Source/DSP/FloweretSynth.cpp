#include "FloweretSynth.h"

namespace Floweret
{
    FloweretSynth::FloweretSynth()
    {
        voices.resize (maxVoices);
    }

    void FloweretSynth::prepare (double sampleRate, int samplesPerBlock)
    {
        currentSampleRate = sampleRate;
        for (auto& v : voices)
            v.prepare (sampleRate);

        colorResonator.prepare (sampleRate);
        effectsRack.prepare (sampleRate);
        arpeggiator.prepare (sampleRate);
        clipSequencer.prepare (sampleRate);

        subDirectBuffer.setSize (2, samplesPerBlock);
        reset();
    }

    void FloweretSynth::reset()
    {
        for (auto& v : voices)
            v.reset();

        colorResonator.reset();
        effectsRack.reset();
        arpeggiator.reset();
        clipSequencer.reset();
        subDirectBuffer.clear();
        currentPitchWheel = 8192;
    }

    int FloweretSynth::getActiveVoiceCount() const noexcept
    {
        int count = 0;
        for (const auto& v : voices)
        {
            if (v.isActive())
                count++;
        }
        return count;
    }

    FloweretVoice* FloweretSynth::findFreeVoice()
    {
        // 1. Look for an inactive voice
        for (auto& v : voices)
        {
            if (!v.isActive())
                return &v;
        }

        // 2. Steal the first voice if all are busy
        return &voices[0];
    }

    void FloweretSynth::handleNoteOn (int midiNote, float velocity)
    {
        if (velocity <= 0.0001f)
        {
            handleNoteOff (midiNote, velocity);
            return;
        }

        arpeggiator.handleNoteOn (midiNote);

        // Retrigger cleanly if this note is already playing on any voice
        for (auto& v : voices)
        {
            if (v.isActive() && v.getCurrentlyPlayingNote() == midiNote)
            {
                v.stopNote (0.0f, false);
            }
        }

        auto* voice = findFreeVoice();
        if (voice != nullptr)
        {
            voice->startNote (midiNote, velocity, currentPitchWheel);
        }
    }

    void FloweretSynth::handleNoteOff (int midiNote, float velocity)
    {
        arpeggiator.handleNoteOff (midiNote);

        for (auto& v : voices)
        {
            if (v.isActive() && v.getCurrentlyPlayingNote() == midiNote)
            {
                v.stopNote (velocity, true);
            }
        }

        // If arpeggiator has no more held notes, release any active harmonic voices
        if (!arpeggiator.hasNotes())
        {
            for (auto& v : voices)
            {
                if (v.isActive() && (v.getCurrentlyPlayingNote() % 12 == midiNote % 12))
                {
                    v.stopNote (velocity, true);
                }
            }
        }
    }

    void FloweretSynth::handlePitchWheel (int pitchWheelValue)
    {
        currentPitchWheel = pitchWheelValue;
        for (auto& v : voices)
        {
            if (v.isActive())
                v.pitchWheelMoved (currentPitchWheel);
        }
    }

    void FloweretSynth::allNotesOff()
    {
        for (auto& v : voices)
            v.reset();

        arpeggiator.reset();
    }

    void FloweretSynth::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                                     const juce::AudioProcessorValueTreeState& apvts, double bpm)
    {
        const int numSamples = buffer.getNumSamples();
        subDirectBuffer.setSize (2, numSamples, false, false, true);
        subDirectBuffer.clear();

        // 1. Read Global & Resonator parameters
        const int rootKey = static_cast<int> (apvts.getRawParameterValue (ParamIDs::colorKey.getParamID())->load());
        const auto scale = static_cast<MusicalScale> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::colorScale.getParamID())->load()));
        const float colorAmount = apvts.getRawParameterValue (ParamIDs::colorAmount.getParamID())->load();
        const float colorResonance = apvts.getRawParameterValue (ParamIDs::colorResonance.getParamID())->load();
        const float specMorph = apvts.getRawParameterValue (ParamIDs::spectralMorph.getParamID())->load();
        const float specGate = apvts.getRawParameterValue (ParamIDs::spectralGate.getParamID())->load();

        const bool arpActive = apvts.getRawParameterValue (ParamIDs::arpActive.getParamID())->load() > 0.5f;
        const int arpMode = static_cast<int> (apvts.getRawParameterValue (ParamIDs::arpMode.getParamID())->load());
        const int arpRate = static_cast<int> (apvts.getRawParameterValue (ParamIDs::arpRate.getParamID())->load());
        const int arpOctaves = static_cast<int> (apvts.getRawParameterValue (ParamIDs::arpOctaves.getParamID())->load());

        const float seqGlitchProb = apvts.getRawParameterValue (ParamIDs::seqGlitchProb.getParamID())->load();

        const auto distType = static_cast<DistortionType> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::distType.getParamID())->load()));
        const float distDrive = apvts.getRawParameterValue (ParamIDs::distDrive.getParamID())->load();
        const float distMix = apvts.getRawParameterValue (ParamIDs::distMix.getParamID())->load();

        const float delayTime = apvts.getRawParameterValue (ParamIDs::delayTime.getParamID())->load();
        const float delayFeedback = apvts.getRawParameterValue (ParamIDs::delayFeedback.getParamID())->load();
        const float delayMix = apvts.getRawParameterValue (ParamIDs::delayMix.getParamID())->load();

        const float reverbSize = apvts.getRawParameterValue (ParamIDs::reverbSize.getParamID())->load();
        const float reverbDamp = apvts.getRawParameterValue (ParamIDs::reverbDamp.getParamID())->load();
        const float reverbMix = apvts.getRawParameterValue (ParamIDs::reverbMix.getParamID())->load();

        const float masterGainDb = apvts.getRawParameterValue (ParamIDs::masterGain.getParamID())->load();
        const float masterLinearGain = juce::Decibels::decibelsToGain (masterGainDb);

        colorResonator.updateScale (scale, rootKey, colorResonance);

        // 2. Dispatch incoming MIDI events
        for (const auto metadata : midiMessages)
        {
            const auto msg = metadata.getMessage();
            if (msg.isNoteOn (false))
            {
                handleNoteOn (msg.getNoteNumber(), msg.getFloatVelocity());
            }
            else if (msg.isNoteOff (true))
            {
                handleNoteOff (msg.getNoteNumber(), msg.getFloatVelocity());
            }
            else if (msg.isPitchWheel())
            {
                handlePitchWheel (msg.getPitchWheelValue());
            }
            else if (msg.isAllNotesOff() || msg.isAllSoundOff())
            {
                allNotesOff();
            }
            else if (msg.isController())
            {
                if (msg.getControllerNumber() == 120 || msg.getControllerNumber() == 123)
                    allNotesOff();
            }
        }

        // 3. Arpeggiator & Clip Sequencer clock triggers
        if (arpActive && arpeggiator.hasNotes())
        {
            for (int s = 0; s < numSamples; ++s)
            {
                const int arpNote = arpeggiator.processSampleClock (arpActive, arpMode, arpRate, arpOctaves, bpm);
                if (arpNote >= 0)
                {
                    auto* voice = findFreeVoice();
                    if (voice != nullptr)
                        voice->startNote (arpNote, 0.85f, currentPitchWheel);
                }
            }
        }

        // 4. Render Polyphonic Voices
        for (auto& v : voices)
        {
            if (v.isActive())
            {
                v.renderNextBlock (buffer, 0, numSamples, apvts, scale, rootKey, subDirectBuffer);
            }
        }

        // 5 & 6. Process Modular Effects Rack (Dynamic user drag-and-drop order)
        auto* left = buffer.getWritePointer (0);
        auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : left;

        std::vector<int> currentChain;
        {
            juce::ScopedLock sl (synthLock);
            currentChain = effectChain;
        }

        for (int effectId : currentChain)
        {
            switch (effectId)
            {
                case 0: // Tube Saturator (Distortion)
                    effectsRack.processDistortion (buffer, distType, distDrive, distMix);
                    break;

                case 1: // Color Resonator
                {
                    auto* lPtr = buffer.getWritePointer (0);
                    auto* rPtr = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : lPtr;
                    for (int i = 0; i < numSamples; ++i)
                        colorResonator.processStereo (lPtr[i], rPtr[i], colorAmount, specMorph, specGate);
                    break;
                }

                case 2: // Analog Stereo Delay
                    effectsRack.processDelay (buffer, delayTime, delayFeedback, delayMix);
                    break;

                case 3: // Lush Botanica Reverb
                    effectsRack.processReverb (buffer, reverbSize, reverbDamp, reverbMix);
                    break;

                default:
                    break;
            }
        }

        // 7. Sum clean Sub Direct bus into master output
        const auto* subL = subDirectBuffer.getReadPointer (0);
        const auto* subR = (subDirectBuffer.getNumChannels() > 1) ? subDirectBuffer.getReadPointer (1) : subL;

        for (int i = 0; i < numSamples; ++i)
        {
            left[i] = (left[i] + subL[i]) * masterLinearGain;
            right[i] = (right[i] + subR[i]) * masterLinearGain;
        }
    }

    void FloweretSynth::loadSample (const juce::AudioBuffer<float>& sample, bool isGranular)
    {
        const juce::ScopedLock sl (synthLock);
        for (auto& v : voices)
        {
            if (isGranular)
                v.loadSampleForGranular (sample);
            else
                v.loadSampleForSampler (sample);
        }
    }
}
