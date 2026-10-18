#pragma once

#include <JuceHeader.h>
#include <vector>

namespace Floweret
{
    //==============================================================================
    // Musical Scales for Color Bass Resonator and Pitch Quantization
    enum class MusicalScale
    {
        Major = 0,
        NaturalMinor,
        HarmonicMinor,
        Dorian,
        PhrygianDominant,
        PentatonicMinor,
        PentatonicMajor,
        InSen,
        Hirajoshi,
        TotalScales
    };

    inline juce::StringArray getScaleNames()
    {
        return {
            "Major",
            "Natural Minor",
            "Harmonic Minor",
            "Dorian",
            "Phrygian Dominant",
            "Pentatonic Minor",
            "Pentatonic Major",
            "In-Sen (Japanese)",
            "Hirajoshi (Japanese)"
        };
    }

    inline std::vector<int> getScaleIntervals (MusicalScale scale)
    {
        switch (scale)
        {
            case MusicalScale::Major:            return { 0, 2, 4, 5, 7, 9, 11 };
            case MusicalScale::NaturalMinor:     return { 0, 2, 3, 5, 7, 8, 10 };
            case MusicalScale::HarmonicMinor:    return { 0, 2, 3, 5, 7, 8, 11 };
            case MusicalScale::Dorian:           return { 0, 2, 3, 5, 7, 9, 10 };
            case MusicalScale::PhrygianDominant: return { 0, 1, 4, 5, 7, 8, 10 };
            case MusicalScale::PentatonicMinor:  return { 0, 3, 5, 7, 10 };
            case MusicalScale::PentatonicMajor:  return { 0, 2, 4, 7, 9 };
            case MusicalScale::InSen:            return { 0, 1, 5, 7, 10 };
            case MusicalScale::Hirajoshi:        return { 0, 2, 3, 7, 8 };
            default:                             return { 0, 2, 4, 5, 7, 9, 11 };
        }
    }

    inline juce::StringArray getNoteNames()
    {
        return { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    }

    //==============================================================================
    // Wavetable Warp Modes
    enum class WarpMode
    {
        None = 0,
        PhaseDistortion,
        FrequencyModulation,
        RingModulation,
        HardSync,
        BendPlus,
        BendMinus,
        DualWarp,
        TotalWarpModes
    };

    inline juce::StringArray getWarpModeNames()
    {
        return { "None", "Phase Distort (PD)", "FM (Linear)", "Ring Mod", "Hard Sync", "Bend (+)", "Bend (-)", "Dual Warp" };
    }

    //==============================================================================
    // Noise Color Types (based on noise colors.pdf)
    enum class NoiseColor
    {
        White = 0,
        Pink,
        Brown,
        Blue,
        Violet,
        Grey,
        Velvet,
        TotalNoiseColors
    };

    inline juce::StringArray getNoiseColorNames()
    {
        return { "White", "Pink (-3dB/oct)", "Brown (-6dB/oct)", "Blue (+3dB/oct)", "Violet (+6dB/oct)", "Grey (Equal Loudness)", "Velvet (Sparse Impulses)" };
    }

    //==============================================================================
    // Filter Types
    enum class FilterType
    {
        LowPass12 = 0,
        LowPass24,
        HighPass12,
        HighPass24,
        BandPass,
        Notch,
        Comb,
        FormantVowel,
        TotalFilterTypes
    };

    inline juce::StringArray getFilterTypeNames()
    {
        return { "LowPass 12dB", "LowPass 24dB", "HighPass 12dB", "HighPass 24dB", "BandPass", "Notch", "Comb Filter", "Formant / Vowel" };
    }

    //==============================================================================
    // Distortion Types
    enum class DistortionType
    {
        SoftClip = 0,
        HardClip,
        Foldback,
        Bitcrush,
        TubeDrive,
        TotalDistortionTypes
    };

    inline juce::StringArray getDistortionTypeNames()
    {
        return { "Soft Saturate", "Hard Clip", "Foldback (Glitch)", "Bitcrush / Decimate", "Tube Drive" };
    }

    //==============================================================================
    // Parameter ID Constants
    namespace ParamIDs
    {
        // Master & Generative
        inline const juce::ParameterID masterGain      { "master_gain", 1 };
        inline const juce::ParameterID mutateAmount    { "mutate_amount", 1 };
        inline const juce::ParameterID bloomAmount     { "bloom_amount", 1 };
        inline const juce::ParameterID spreadAmount    { "spread_amount", 1 };
        inline const juce::ParameterID tailAmount      { "tail_amount", 1 };
        inline const juce::ParameterID colorAmount     { "color_amount", 1 };

        // Color Bass Resonator
        inline const juce::ParameterID colorKey        { "color_key", 1 };
        inline const juce::ParameterID colorScale      { "color_scale", 1 };
        inline const juce::ParameterID colorResonance  { "color_resonance", 1 };
        inline const juce::ParameterID spectralMorph   { "spectral_morph", 1 };
        inline const juce::ParameterID spectralGate    { "spectral_gate", 1 };

        // Wavetable Oscillator A
        inline const juce::ParameterID wtActive        { "wt_active", 1 };
        inline const juce::ParameterID wtPosition      { "wt_position", 1 };
        inline const juce::ParameterID wtWarpMode      { "wt_warp_mode", 1 };
        inline const juce::ParameterID wtWarpAmount    { "wt_warp_amount", 1 };
        inline const juce::ParameterID wtSmoothInterp  { "wt_smooth_interp", 1 };
        inline const juce::ParameterID wtDetune        { "wt_detune", 1 };
        inline const juce::ParameterID wtTune          { "wt_detune", 1 };
        inline const juce::ParameterID wtPan           { "wt_pan", 1 };
        inline const juce::ParameterID wtLevel         { "wt_level", 1 };
        inline const juce::ParameterID wtUnison        { "wt_unison", 1 };
        inline const juce::ParameterID wtDetuneFine    { "wt_detune_fine", 1 };
        inline const juce::ParameterID wtMix           { "wt_mix", 1 };

        // Wavetable Oscillator B
        inline const juce::ParameterID wtBActive       { "wt_b_active", 1 };
        inline const juce::ParameterID wtBPosition     { "wt_b_position", 1 };
        inline const juce::ParameterID wtBWarpMode     { "wt_b_warp_mode", 1 };
        inline const juce::ParameterID wtBWarpAmount   { "wt_b_warp_amount", 1 };
        inline const juce::ParameterID wtBSmoothInterp { "wt_b_smooth_interp", 1 };
        inline const juce::ParameterID wtBDetune       { "wt_b_detune", 1 };
        inline const juce::ParameterID wtBTune         { "wt_b_detune", 1 };
        inline const juce::ParameterID wtBPan          { "wt_b_pan", 1 };
        inline const juce::ParameterID wtBLevel        { "wt_b_level", 1 };
        inline const juce::ParameterID wtBUnison       { "wt_b_unison", 1 };
        inline const juce::ParameterID wtBDetuneFine   { "wt_b_detune_fine", 1 };
        inline const juce::ParameterID wtBMix          { "wt_b_mix", 1 };

        // Wavetable Oscillator C
        inline const juce::ParameterID wtCActive       { "wt_c_active", 1 };
        inline const juce::ParameterID wtCPosition     { "wt_c_position", 1 };
        inline const juce::ParameterID wtCWarpMode     { "wt_c_warp_mode", 1 };
        inline const juce::ParameterID wtCWarpAmount   { "wt_c_warp_amount", 1 };
        inline const juce::ParameterID wtCSmoothInterp { "wt_c_smooth_interp", 1 };
        inline const juce::ParameterID wtCDetune       { "wt_c_detune", 1 };
        inline const juce::ParameterID wtCTune         { "wt_c_detune", 1 };
        inline const juce::ParameterID wtCPan          { "wt_c_pan", 1 };
        inline const juce::ParameterID wtCLevel        { "wt_c_level", 1 };
        inline const juce::ParameterID wtCUnison       { "wt_c_unison", 1 };
        inline const juce::ParameterID wtCDetuneFine   { "wt_c_detune_fine", 1 };
        inline const juce::ParameterID wtCMix          { "wt_c_mix", 1 };

        // Granular Oscillator
        inline const juce::ParameterID granActive      { "gran_active", 1 };
        inline const juce::ParameterID granPosition    { "gran_position", 1 };
        inline const juce::ParameterID granSize        { "gran_size", 1 };
        inline const juce::ParameterID granDensity     { "gran_density", 1 };
        inline const juce::ParameterID granSpray       { "gran_spray", 1 };
        inline const juce::ParameterID granPitchJitter { "gran_pitch_jitter", 1 };
        inline const juce::ParameterID granFreeze      { "gran_freeze", 1 };
        inline const juce::ParameterID granLevel       { "gran_level", 1 };

        // Spectral Oscillator
        inline const juce::ParameterID specActive      { "spec_active", 1 };
        inline const juce::ParameterID specHarmonics   { "spec_harmonics", 1 };
        inline const juce::ParameterID specQuantize    { "spec_quantize", 1 };
        inline const juce::ParameterID specTilt        { "spec_tilt", 1 };
        inline const juce::ParameterID specSmear       { "spec_smear", 1 };
        inline const juce::ParameterID specLevel       { "spec_level", 1 };

        // Sample Oscillator
        inline const juce::ParameterID smpActive       { "smp_active", 1 };
        inline const juce::ParameterID smpLoopMode     { "smp_loop_mode", 1 };
        inline const juce::ParameterID smpTapeStop     { "smp_tape_stop", 1 };
        inline const juce::ParameterID smpSliceIndex   { "smp_slice_index", 1 };
        inline const juce::ParameterID smpLevel        { "smp_level", 1 };

        // Multisample Oscillator
        inline const juce::ParameterID msmpActive      { "msmp_active", 1 };
        inline const juce::ParameterID msmpPreset      { "msmp_preset", 1 };
        inline const juce::ParameterID msmpLevel       { "msmp_level", 1 };

        // Noise Oscillator
        inline const juce::ParameterID noiseActive     { "noise_active", 1 };
        inline const juce::ParameterID noiseColor      { "noise_color", 1 };
        inline const juce::ParameterID noiseLevel      { "noise_level", 1 };

        // Sub Oscillator
        inline const juce::ParameterID subActive       { "sub_active", 1 };
        inline const juce::ParameterID subOctave       { "sub_octave", 1 };
        inline const juce::ParameterID subDrive        { "sub_drive", 1 };
        inline const juce::ParameterID subDirectOut    { "sub_direct_out", 1 };
        inline const juce::ParameterID subLevel        { "sub_level", 1 };

        // Filter
        inline const juce::ParameterID filterType      { "filter_type", 1 };
        inline const juce::ParameterID filterCutoff    { "filter_cutoff", 1 };
        inline const juce::ParameterID filterResonance { "filter_resonance", 1 };
        inline const juce::ParameterID filterDrive     { "filter_drive", 1 };

        // Amp Envelope
        inline const juce::ParameterID envAttack       { "env_attack", 1 };
        inline const juce::ParameterID envDecay        { "env_decay", 1 };
        inline const juce::ParameterID envSustain      { "env_sustain", 1 };
        inline const juce::ParameterID envRelease      { "env_release", 1 };

        // Filter Envelope
        inline const juce::ParameterID fenvAttack      { "fenv_attack", 1 };
        inline const juce::ParameterID fenvDecay       { "fenv_decay", 1 };
        inline const juce::ParameterID fenvSustain     { "fenv_sustain", 1 };
        inline const juce::ParameterID fenvRelease     { "fenv_release", 1 };
        inline const juce::ParameterID fenvAmount      { "fenv_amount", 1 };

        // LFO 1
        inline const juce::ParameterID lfoRate         { "lfo_rate", 1 };
        inline const juce::ParameterID lfoShape        { "lfo_shape", 1 };
        inline const juce::ParameterID lfoChaos        { "lfo_chaos", 1 };

        // Effects Rack
        inline const juce::ParameterID distType        { "dist_type", 1 };
        inline const juce::ParameterID distDrive       { "dist_drive", 1 };
        inline const juce::ParameterID distMix         { "dist_mix", 1 };

        inline const juce::ParameterID delayTime       { "delay_time", 1 };
        inline const juce::ParameterID delayFeedback   { "delay_feedback", 1 };
        inline const juce::ParameterID delayMix        { "delay_mix", 1 };

        inline const juce::ParameterID reverbSize      { "reverb_size", 1 };
        inline const juce::ParameterID reverbDamp      { "reverb_damp", 1 };
        inline const juce::ParameterID reverbMix       { "reverb_mix", 1 };

        // Arpeggiator & Sequencer
        inline const juce::ParameterID arpActive       { "arp_active", 1 };
        inline const juce::ParameterID arpMode         { "arp_mode", 1 };
        inline const juce::ParameterID arpRate         { "arp_rate", 1 };
        inline const juce::ParameterID arpOctaves      { "arp_octaves", 1 };
        inline const juce::ParameterID seqGlitchProb   { "seq_glitch_prob", 1 };
    }

    //==============================================================================
    // Helper to create the complete parameter layout for AudioProcessorValueTreeState
    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // Master & Generative
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::masterGain, "Master Gain", juce::NormalisableRange<float> (-60.0f, 6.0f, 0.1f, 2.0f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::mutateAmount, "Mutate", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::bloomAmount, "Bloom", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::spreadAmount, "Spread", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.25f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::tailAmount, "Tail", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.20f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::colorAmount, "Color", juce::NormalisableRange<float> (0.0f, 200.0f, 0.1f), 30.0f));

        // Color Bass Resonator
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::colorKey, "Root Key", getNoteNames(), 3)); // Default Eb (index 3)
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::colorScale, "Scale", getScaleNames(), 0)); // Major
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::colorResonance, "Resonance", juce::NormalisableRange<float> (0.1f, 30.0f, 0.1f, 0.5f), 8.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::spectralMorph, "Spectral Morph", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::spectralGate, "Spectral Gate", juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -40.0f));

        // Wavetable Oscillator A
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::wtActive, "WT A Active", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtPosition, "WT A Position", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::wtWarpMode, "WT A Warp Mode", getWarpModeNames(), 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtWarpAmount, "WT A Warp Amount", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::wtSmoothInterp, "WT A Smooth Interp", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtDetune, "WT A Detune/Tune", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtPan, "WT A Pan", juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtLevel, "WT A Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (
            ParamIDs::wtUnison, "WT A Unison", 1, 16, 1));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtDetuneFine, "WT A Detune Fine", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtMix, "WT A Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.85f));

        // Wavetable Oscillator B
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::wtBActive, "WT B Active", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBPosition, "WT B Position", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.25f));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::wtBWarpMode, "WT B Warp Mode", getWarpModeNames(), 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBWarpAmount, "WT B Warp Amount", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::wtBSmoothInterp, "WT B Smooth Interp", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBDetune, "WT B Detune/Tune", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBPan, "WT B Pan", juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBLevel, "WT B Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (
            ParamIDs::wtBUnison, "WT B Unison", 1, 16, 1));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBDetuneFine, "WT B Detune Fine", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtBMix, "WT B Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.85f));

        // Wavetable Oscillator C
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::wtCActive, "WT C Active", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCPosition, "WT C Position", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::wtCWarpMode, "WT C Warp Mode", getWarpModeNames(), 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCWarpAmount, "WT C Warp Amount", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::wtCSmoothInterp, "WT C Smooth Interp", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCTune, "WT C Detune/Tune", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCPan, "WT C Pan", juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCLevel, "WT C Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (
            ParamIDs::wtCUnison, "WT C Unison", 1, 16, 1));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCDetuneFine, "WT C Detune Fine", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::wtCMix, "WT C Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.85f));

        // Granular Oscillator
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::granActive, "Granular Active", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::granPosition, "Gran Position", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.25f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::granSize, "Gran Size (ms)", juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.4f), 60.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::granDensity, "Gran Density", juce::NormalisableRange<float> (2.0f, 64.0f, 1.0f), 24.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::granSpray, "Gran Spray", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::granPitchJitter, "Gran Pitch Jitter", juce::NormalisableRange<float> (0.0f, 12.0f, 0.1f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::granFreeze, "Gran Freeze", false));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::granLevel, "Gran Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));

        // Spectral Oscillator
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::specActive, "Spectral Active", false));
        params.push_back (std::make_unique<juce::AudioParameterInt> (
            ParamIDs::specHarmonics, "Harmonics", 1, 64, 32));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::specQuantize, "Harmonic Quantize", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::specTilt, "Spectral Tilt", juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::specSmear, "Spectral Smear", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::specLevel, "Spectral Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));

        // Sample Oscillator
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::smpActive, "Sample Active", false));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::smpLoopMode, "Loop Mode", juce::StringArray { "One-Shot", "Forward Loop", "Ping-Pong" }, 1));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::smpTapeStop, "Tape-Stop Inertia", juce::NormalisableRange<float> (0.0f, 2.0f, 0.01f), 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (
            ParamIDs::smpSliceIndex, "Slice Index", 0, 16, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::smpLevel, "Sample Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));

        // Multisample Oscillator
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::msmpActive, "Multisample Active", false));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::msmpPreset, "Multisample Bank", juce::StringArray { "Botanica Bells", "Organic Kalimba", "Pizzicato Flora", "Ethereal Flute" }, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::msmpLevel, "Multisample Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));

        // Noise Oscillator
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::noiseActive, "Noise Active", true));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::noiseColor, "Noise Color", getNoiseColorNames(), static_cast<int> (NoiseColor::Velvet)));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::noiseLevel, "Noise Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));

        // Sub Oscillator
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::subActive, "Sub Active", true));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::subOctave, "Sub Octave", juce::StringArray { "-1 Oct", "-2 Oct", "-3 Oct" }, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::subDrive, "Sub Drive", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.1f));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::subDirectOut, "Sub Direct Out", true));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::subLevel, "Sub Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.6f));

        // Filter
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::filterType, "Filter Type", getFilterTypeNames(), 1)); // LP 24dB
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::filterCutoff, "Cutoff", juce::NormalisableRange<float> (20.0f, 20000.0f, 0.1f, 0.3f), 4500.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::filterResonance, "Resonance", juce::NormalisableRange<float> (0.1f, 10.0f, 0.05f), 1.2f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::filterDrive, "Filter Drive", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

        // Amp Envelope
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::envAttack, "Attack", juce::NormalisableRange<float> (0.001f, 10.0f, 0.001f, 0.3f), 0.015f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::envDecay, "Decay", juce::NormalisableRange<float> (0.01f, 10.0f, 0.001f, 0.3f), 0.45f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::envSustain, "Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.75f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::envRelease, "Release", juce::NormalisableRange<float> (0.01f, 10.0f, 0.001f, 0.3f), 0.5f));

        // Filter Envelope
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::fenvAttack, "Filter Attack", juce::NormalisableRange<float> (0.001f, 5.0f, 0.001f, 0.3f), 0.005f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::fenvDecay, "Filter Decay", juce::NormalisableRange<float> (0.01f, 5.0f, 0.001f, 0.3f), 0.35f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::fenvSustain, "Filter Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.2f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::fenvRelease, "Filter Release", juce::NormalisableRange<float> (0.01f, 5.0f, 0.001f, 0.3f), 0.4f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::fenvAmount, "Filter Env Amount", juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.5f));

        // LFO 1
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::lfoRate, "LFO Rate (Hz)", juce::NormalisableRange<float> (0.05f, 30.0f, 0.01f, 0.3f), 1.5f));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::lfoShape, "LFO Shape", juce::StringArray { "Sine", "Triangle", "Saw Down", "Saw Up", "Square", "Random S&H" }, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::lfoChaos, "LFO Chaos/Smooth", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

        // Effects Rack
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::distType, "Distortion Type", getDistortionTypeNames(), 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::distDrive, "Distortion Drive", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.2f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::distMix, "Distortion Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::delayTime, "Delay Time (s)", juce::NormalisableRange<float> (0.01f, 2.0f, 0.001f, 0.5f), 0.35f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::delayFeedback, "Delay Feedback", juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f), 0.45f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::delayMix, "Delay Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.25f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::reverbSize, "Reverb Size", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::reverbDamp, "Reverb Damp", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::reverbMix, "Reverb Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.35f));

        // Arpeggiator & Sequencer
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            ParamIDs::arpActive, "Arpeggiator Active", false));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::arpMode, "Arp Pattern", juce::StringArray { "Up", "Down", "Up/Down", "Random", "Chord" }, 0));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParamIDs::arpRate, "Arp Rate", juce::StringArray { "1/4", "1/8", "1/16", "1/32", "1/8T", "1/16T" }, 2));
        params.push_back (std::make_unique<juce::AudioParameterInt> (
            ParamIDs::arpOctaves, "Arp Octave Range", 1, 4, 1));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParamIDs::seqGlitchProb, "Glitch Probability", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

        return { params.begin(), params.end() };
    }
}
