#include "FloweretVoice.h"

namespace Floweret
{
    FloweretVoice::FloweretVoice()
    {
    }

    void FloweretVoice::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        wavetableOscA.prepare (sampleRate);
        wavetableOscB.prepare (sampleRate);
        wavetableOscC.prepare (sampleRate);
        granularOsc.prepare (sampleRate);
        spectralOsc.prepare (sampleRate);
        sampleOsc.prepare (sampleRate);
        multisampleOsc.prepare (sampleRate);
        noiseOsc.prepare (sampleRate);
        subOsc.prepare (sampleRate);

        voiceFilter.prepare (sampleRate);
        ampEnvelope.prepare (sampleRate);
        filterEnvelope.prepare (sampleRate);
        voiceLfo.prepare (sampleRate);

        reset();
    }

    void FloweretVoice::reset()
    {
        currentMidiNote = -1;
        currentVelocity = 0.0f;
        pitchBendSemitones = 0.0f;

        wavetableOscA.reset();
        wavetableOscB.reset();
        wavetableOscC.reset();
        granularOsc.reset();
        spectralOsc.reset();
        sampleOsc.reset();
        multisampleOsc.reset();
        noiseOsc.reset();
        subOsc.reset();

        voiceFilter.reset();
        ampEnvelope.reset();
        filterEnvelope.reset();
        voiceLfo.reset();
    }

    void FloweretVoice::startNote (int midiNoteNumber, float velocity, int currentPitchWheelPosition)
    {
        currentMidiNote = midiNoteNumber;
        currentVelocity = velocity;
        pitchWheelMoved (currentPitchWheelPosition);

        const float noteWithBend = static_cast<float> (currentMidiNote) + pitchBendSemitones;
        const float freq = midiToFreq (noteWithBend);

        wavetableOscA.setFrequency (freq);
        wavetableOscB.setFrequency (freq);
        wavetableOscC.setFrequency (freq);
        granularOsc.setFrequency (freq);
        spectralOsc.setFrequency (freq);
        sampleOsc.setFrequency (freq);
        multisampleOsc.setNoteAndVelocity (currentMidiNote, currentVelocity);
        subOsc.setFrequency (freq);

        ampEnvelope.noteOn();
        filterEnvelope.noteOn();
    }

    void FloweretVoice::stopNote (float velocity, bool allowTailOff)
    {
        if (allowTailOff)
        {
            ampEnvelope.noteOff();
            filterEnvelope.noteOff();
            if (!ampEnvelope.isActive())
                reset();
        }
        else
        {
            reset();
        }
    }

    void FloweretVoice::pitchWheelMoved (int newPitchWheelValue)
    {
        // 8192 is center; +/- 2 semitone range
        pitchBendSemitones = (static_cast<float> (newPitchWheelValue - 8192) / 8192.0f) * 2.0f;

        if (currentMidiNote >= 0)
        {
            const float noteWithBend = static_cast<float> (currentMidiNote) + pitchBendSemitones;
            const float freq = midiToFreq (noteWithBend);
            wavetableOscA.setFrequency (freq);
            wavetableOscB.setFrequency (freq);
            wavetableOscC.setFrequency (freq);
            granularOsc.setFrequency (freq);
            spectralOsc.setFrequency (freq);
            sampleOsc.setFrequency (freq);
            subOsc.setFrequency (freq);
        }
    }

    bool FloweretVoice::isActive() const noexcept
    {
        return ampEnvelope.isActive();
    }

    void FloweretVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples,
                                        const juce::AudioProcessorValueTreeState& apvts,
                                        MusicalScale activeScale, int rootKey,
                                        juce::AudioBuffer<float>& subDirectBuffer)
    {
        if (!isActive())
            return;

        // Fetch parameters from APVTS for OSC A, B, and C
        const bool wtActive = apvts.getRawParameterValue (ParamIDs::wtActive.getParamID())->load() > 0.5f;
        const float wtPos = apvts.getRawParameterValue (ParamIDs::wtPosition.getParamID())->load();
        const auto wtWarpMode = static_cast<WarpMode> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::wtWarpMode.getParamID())->load()));
        const float wtWarpAmount = apvts.getRawParameterValue (ParamIDs::wtWarpAmount.getParamID())->load();
        const bool wtSmooth = apvts.getRawParameterValue (ParamIDs::wtSmoothInterp.getParamID())->load() > 0.5f;
        const float wtTune = apvts.getRawParameterValue (ParamIDs::wtDetune.getParamID())->load();
        const float wtPan = apvts.getRawParameterValue (ParamIDs::wtPan.getParamID())->load();
        const float wtLevel = apvts.getRawParameterValue (ParamIDs::wtLevel.getParamID())->load();
        const int wtUnison = static_cast<int> (apvts.getRawParameterValue (ParamIDs::wtUnison.getParamID())->load());
        const float wtDetuneFine = apvts.getRawParameterValue (ParamIDs::wtDetuneFine.getParamID())->load();
        const float wtMix = apvts.getRawParameterValue (ParamIDs::wtMix.getParamID())->load();

        const bool wtBActive = apvts.getRawParameterValue (ParamIDs::wtBActive.getParamID())->load() > 0.5f;
        const float wtBPos = apvts.getRawParameterValue (ParamIDs::wtBPosition.getParamID())->load();
        const auto wtBWarpMode = static_cast<WarpMode> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::wtBWarpMode.getParamID())->load()));
        const float wtBWarpAmount = apvts.getRawParameterValue (ParamIDs::wtBWarpAmount.getParamID())->load();
        const bool wtBSmooth = apvts.getRawParameterValue (ParamIDs::wtBSmoothInterp.getParamID())->load() > 0.5f;
        const float wtBTune = apvts.getRawParameterValue (ParamIDs::wtBDetune.getParamID())->load();
        const float wtBPan = apvts.getRawParameterValue (ParamIDs::wtBPan.getParamID())->load();
        const float wtBLevel = apvts.getRawParameterValue (ParamIDs::wtBLevel.getParamID())->load();
        const int wtBUnison = static_cast<int> (apvts.getRawParameterValue (ParamIDs::wtBUnison.getParamID())->load());
        const float wtBDetuneFine = apvts.getRawParameterValue (ParamIDs::wtBDetuneFine.getParamID())->load();
        const float wtBMix = apvts.getRawParameterValue (ParamIDs::wtBMix.getParamID())->load();

        const bool wtCActive = apvts.getRawParameterValue (ParamIDs::wtCActive.getParamID())->load() > 0.5f;
        const float wtCPos = apvts.getRawParameterValue (ParamIDs::wtCPosition.getParamID())->load();
        const auto wtCWarpMode = static_cast<WarpMode> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::wtCWarpMode.getParamID())->load()));
        const float wtCWarpAmount = apvts.getRawParameterValue (ParamIDs::wtCWarpAmount.getParamID())->load();
        const bool wtCSmooth = apvts.getRawParameterValue (ParamIDs::wtCSmoothInterp.getParamID())->load() > 0.5f;
        const float wtCTune = apvts.getRawParameterValue (ParamIDs::wtCTune.getParamID())->load();
        const float wtCPan = apvts.getRawParameterValue (ParamIDs::wtCPan.getParamID())->load();
        const float wtCLevel = apvts.getRawParameterValue (ParamIDs::wtCLevel.getParamID())->load();
        const int wtCUnison = static_cast<int> (apvts.getRawParameterValue (ParamIDs::wtCUnison.getParamID())->load());
        const float wtCDetuneFine = apvts.getRawParameterValue (ParamIDs::wtCDetuneFine.getParamID())->load();
        const float wtCMix = apvts.getRawParameterValue (ParamIDs::wtCMix.getParamID())->load();

        const float baseNote = static_cast<float> (currentMidiNote) + pitchBendSemitones;
        wavetableOscA.setFrequency (midiToFreq (baseNote + wtTune));
        wavetableOscB.setFrequency (midiToFreq (baseNote + wtBTune));
        wavetableOscC.setFrequency (midiToFreq (baseNote + wtCTune));

        const bool granActive = apvts.getRawParameterValue (ParamIDs::granActive.getParamID())->load() > 0.5f;
        const float granPos = apvts.getRawParameterValue (ParamIDs::granPosition.getParamID())->load();
        const float granSize = apvts.getRawParameterValue (ParamIDs::granSize.getParamID())->load();
        const float granDensity = apvts.getRawParameterValue (ParamIDs::granDensity.getParamID())->load();
        const float granSpray = apvts.getRawParameterValue (ParamIDs::granSpray.getParamID())->load();
        const float granPitchJitter = apvts.getRawParameterValue (ParamIDs::granPitchJitter.getParamID())->load();
        const bool granFreeze = apvts.getRawParameterValue (ParamIDs::granFreeze.getParamID())->load() > 0.5f;
        const float granLevel = apvts.getRawParameterValue (ParamIDs::granLevel.getParamID())->load();

        const bool specActive = apvts.getRawParameterValue (ParamIDs::specActive.getParamID())->load() > 0.5f;
        const int specHarmonics = static_cast<int> (apvts.getRawParameterValue (ParamIDs::specHarmonics.getParamID())->load());
        const float specQuantize = apvts.getRawParameterValue (ParamIDs::specQuantize.getParamID())->load();
        const float specTilt = apvts.getRawParameterValue (ParamIDs::specTilt.getParamID())->load();
        const float specSmear = apvts.getRawParameterValue (ParamIDs::specSmear.getParamID())->load();
        const float specLevel = apvts.getRawParameterValue (ParamIDs::specLevel.getParamID())->load();

        const bool smpActive = apvts.getRawParameterValue (ParamIDs::smpActive.getParamID())->load() > 0.5f;
        const int smpLoopMode = static_cast<int> (apvts.getRawParameterValue (ParamIDs::smpLoopMode.getParamID())->load());
        const float smpTapeStop = apvts.getRawParameterValue (ParamIDs::smpTapeStop.getParamID())->load();
        const int smpSliceIndex = static_cast<int> (apvts.getRawParameterValue (ParamIDs::smpSliceIndex.getParamID())->load());
        const float smpLevel = apvts.getRawParameterValue (ParamIDs::smpLevel.getParamID())->load();

        const bool msmpActive = apvts.getRawParameterValue (ParamIDs::msmpActive.getParamID())->load() > 0.5f;
        const float msmpLevel = apvts.getRawParameterValue (ParamIDs::msmpLevel.getParamID())->load();

        const bool noiseActive = apvts.getRawParameterValue (ParamIDs::noiseActive.getParamID())->load() > 0.5f;
        const auto noiseColor = static_cast<NoiseColor> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::noiseColor.getParamID())->load()));
        const float noiseLevel = apvts.getRawParameterValue (ParamIDs::noiseLevel.getParamID())->load();

        const bool subActive = apvts.getRawParameterValue (ParamIDs::subActive.getParamID())->load() > 0.5f;
        const int subOctave = static_cast<int> (apvts.getRawParameterValue (ParamIDs::subOctave.getParamID())->load());
        const float subDrive = apvts.getRawParameterValue (ParamIDs::subDrive.getParamID())->load();
        const bool subDirect = apvts.getRawParameterValue (ParamIDs::subDirectOut.getParamID())->load() > 0.5f;
        const float subLevel = apvts.getRawParameterValue (ParamIDs::subLevel.getParamID())->load();

        const auto filterType = static_cast<FilterType> (static_cast<int> (apvts.getRawParameterValue (ParamIDs::filterType.getParamID())->load()));
        const float baseCutoff = apvts.getRawParameterValue (ParamIDs::filterCutoff.getParamID())->load();
        const float filterRes = apvts.getRawParameterValue (ParamIDs::filterResonance.getParamID())->load();
        const float filterDrive = apvts.getRawParameterValue (ParamIDs::filterDrive.getParamID())->load();

        const float envA = apvts.getRawParameterValue (ParamIDs::envAttack.getParamID())->load();
        const float envD = apvts.getRawParameterValue (ParamIDs::envDecay.getParamID())->load();
        const float envS = apvts.getRawParameterValue (ParamIDs::envSustain.getParamID())->load();
        const float envR = apvts.getRawParameterValue (ParamIDs::envRelease.getParamID())->load();

        const float fenvA = apvts.getRawParameterValue (ParamIDs::fenvAttack.getParamID())->load();
        const float fenvD = apvts.getRawParameterValue (ParamIDs::fenvDecay.getParamID())->load();
        const float fenvS = apvts.getRawParameterValue (ParamIDs::fenvSustain.getParamID())->load();
        const float fenvR = apvts.getRawParameterValue (ParamIDs::fenvRelease.getParamID())->load();
        const float fenvAmount = apvts.getRawParameterValue (ParamIDs::fenvAmount.getParamID())->load();

        const float lfoRate = apvts.getRawParameterValue (ParamIDs::lfoRate.getParamID())->load();
        const int lfoShape = static_cast<int> (apvts.getRawParameterValue (ParamIDs::lfoShape.getParamID())->load());
        const float lfoChaos = apvts.getRawParameterValue (ParamIDs::lfoChaos.getParamID())->load();

        auto* outL = outputBuffer.getWritePointer (0);
        auto* outR = (outputBuffer.getNumChannels() > 1) ? outputBuffer.getWritePointer (1) : outL;

        auto* subOutL = subDirectBuffer.getWritePointer (0);
        auto* subOutR = (subDirectBuffer.getNumChannels() > 1) ? subDirectBuffer.getWritePointer (1) : subOutL;

        for (int i = 0; i < numSamples; ++i)
        {
            const float ampEnvVal = ampEnvelope.getNextSample (envA, envD, envS, envR);
            const float fEnvVal = filterEnvelope.getNextSample (fenvA, fenvD, fenvS, fenvR);
            const float lfoVal = voiceLfo.getNextSample (lfoRate, lfoShape, lfoChaos);

            if (!ampEnvelope.isActive())
            {
                reset();
                break;
            }

            // Generate Sub Oscillator sample
            float subSample = 0.0f;
            if (subActive)
            {
                subSample = subOsc.getNextSample (subOctave, subDrive, 0) * subLevel * currentVelocity;
                if (subDirect)
                {
                    subOutL[startSample + i] += subSample * ampEnvVal;
                    subOutR[startSample + i] += subSample * ampEnvVal;
                }
            }

            // Generate Wavetable stereo samples for A, B, and C
            float wtSampleL = 0.0f;
            float wtSampleR = 0.0f;

            if (wtActive)
            {
                const float modPos = std::max (0.0f, std::min (wtPos + lfoVal * 0.15f, 1.0f));
                const auto stereo = wavetableOscA.getNextSampleStereo (modPos, wtWarpMode, wtWarpAmount, wtSmooth,
                                                                      wtUnison, wtDetuneFine, wtPan, subSample);
                const float gainA = wtLevel * wtMix;
                wtSampleL += stereo.first * gainA;
                wtSampleR += stereo.second * gainA;
            }

            if (wtBActive)
            {
                const float modPos = std::max (0.0f, std::min (wtBPos + lfoVal * 0.15f, 1.0f));
                const auto stereo = wavetableOscB.getNextSampleStereo (modPos, wtBWarpMode, wtBWarpAmount, wtBSmooth,
                                                                      wtBUnison, wtBDetuneFine, wtBPan);
                const float gainB = wtBLevel * wtBMix;
                wtSampleL += stereo.first * gainB;
                wtSampleR += stereo.second * gainB;
            }

            if (wtCActive)
            {
                const float modPos = std::max (0.0f, std::min (wtCPos + lfoVal * 0.15f, 1.0f));
                const auto stereo = wavetableOscC.getNextSampleStereo (modPos, wtCWarpMode, wtCWarpAmount, wtCSmooth,
                                                                      wtCUnison, wtCDetuneFine, wtCPan);
                const float gainC = wtCLevel * wtCMix;
                wtSampleL += stereo.first * gainC;
                wtSampleR += stereo.second * gainC;
            }

            // Generate Granular cloud sample
            float granSampleL = 0.0f;
            float granSampleR = 0.0f;
            if (granActive)
            {
                const auto granPair = granularOsc.getNextSample (granPos, granSize, granDensity, granSpray, granPitchJitter, granFreeze);
                granSampleL = granPair.first * granLevel;
                granSampleR = granPair.second * granLevel;
            }

            // Generate Spectral Oscillator sample
            float specSample = 0.0f;
            if (specActive)
            {
                specSample = spectralOsc.getNextSample (specHarmonics, specQuantize, specTilt, specSmear, activeScale, rootKey) * specLevel;
            }

            // Generate Sample Oscillator sample
            float smpSample = 0.0f;
            if (smpActive)
            {
                smpSample = sampleOsc.getNextSample (smpLoopMode, smpTapeStop, smpSliceIndex) * smpLevel;
            }

            // Generate Multisample Oscillator sample
            float msmpSample = 0.0f;
            if (msmpActive)
            {
                msmpSample = multisampleOsc.getNextSample() * msmpLevel;
            }

            // Generate Noise sample
            float noiseSample = 0.0f;
            if (noiseActive)
            {
                noiseSample = noiseOsc.getNextSample (noiseColor) * noiseLevel;
            }

            // Mix pre-filter audio
            const float wtMono = (wtSampleL + wtSampleR) * 0.5f;
            const float monoMix = wtMono + specSample + smpSample + msmpSample + noiseSample
                                + ((subActive && !subDirect) ? subSample : 0.0f);

            // Modulate filter cutoff with filter envelope
            const float modCutoff = std::max (20.0f, std::min (baseCutoff * std::pow (2.0f, fEnvVal * fenvAmount * 5.0f), 20000.0f));
            const float filteredMono = voiceFilter.processSample (monoMix, filterType, modCutoff, filterRes, filterDrive);

            // Apply Amp Envelope & Velocity
            const float voiceGain = ampEnvVal * currentVelocity;
            const float leftVoice = (filteredMono + (wtSampleL - wtMono) + granSampleL) * voiceGain;
            const float rightVoice = (filteredMono + (wtSampleR - wtMono) + granSampleR) * voiceGain;

            outL[startSample + i] += leftVoice;
            outR[startSample + i] += rightVoice;
        }
    }
}
