# Floweret User Manual & Sound Design Guide

Welcome to the official documentation for **Floweret** — a generative botanical synthesizer for **Color Bass**, **Botanica**, **Dubstep**, and **Glitch**.

---

## 📚 Table of Contents

1. [Chapter 0: Overview & Quickstart Guide](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Manual/00_OVERVIEW_AND_QUICKSTART.md)
   - Introduction to Floweret & target genres
   - The GRASS design aesthetic & color palette
   - Visual interface tour (Header, Toolbar, Growth Field, Keyboard)
   - Quickstart: your first patch in 5 steps

2. [Chapter 1: Oscillator Suite & Sound Engines](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Manual/01_OSCILLATORS.md)
   - Wavetable Oscillator (2,048-sample frames, Hermite interpolation, Warp modes: PD, FM, RM, Hard Sync, Bend, Dual Warp)
   - Granular Oscillator (64-grain cloud, spray, density, jitter, freeze, windowing, stereo pan)
   - Spectral Oscillator (Additive harmonic resynthesis, Color Bass scale quantizer, tilt, smear)
   - Sample Oscillator (Snap-to-zero loops, loop modes, tape-stop inertia, transient slicing)
   - Multisample Oscillator (Multi-zone key & velocity maps, SFZ architecture, factory botanica banks)
   - Colored Noise Oscillator (White, Pink, Brown, Blue, Violet, Grey, Velvet noise)
   - Sub Oscillator (Octave offsets, saturation drive, Sub Direct bypass routing)

3. [Chapter 2: Color Bass Engine & Harmonic Resonators](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Manual/02_COLOR_BASS_AND_HARMONICS.md)
   - What is Color Bass? (Origin and principles)
   - The scale-locked resonator architecture (inspired by Xynth Audio's Chroma)
   - Scale selection & frequency tuning across octaves
   - The Color parameter (0–100% dry/wet vs. 100–200% singing formant overdrive)
   - Spectral Morph & Spectral Gate
   - Sound design recipes: Chime-style leads, Future Riddim growls, Botanica bells

4. [Chapter 3: Modulation, Arpeggiator & Clip Sequencer](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Manual/03_MODULATION_AND_SEQUENCING.md)
   - Sample-accurate Amp and Filter Envelope Generators
   - Multi-shape LFOs with organic Smooth Chaos drift
   - Multi-pattern Arpeggiator (Up, Down, Up/Down, Random, Chord, Rate divisions)
   - 16-Step Clip Sequencer with pitch offsets, velocity, gate, and Glitch probability

5. [Chapter 4: Filters, Effects Rack & Signal Routing](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Manual/04_EFFECTS_AND_ROUTING.md)
   - Master signal flow diagram
   - Multi-Mode SVF Filter (LP12/24, HP12/24, BP, Notch, Comb, Formant / Vowel)
   - Waveshaping Distortion (Soft, Hard, Foldback Glitch, Bitcrush, Tube)
   - Stereo Ping-Pong Delay with cross-feedback
   - Algorithmic Diffusion Reverb ("Botanica Bloom")
   - Sub-Bass Direct Out routing for phase-perfect mono sub foundation

6. [Chapter 5: Developer & Build Guide (VS Code & CMake)](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Manual/05_DEVELOPER_AND_BUILD_GUIDE.md)
   - Building on Windows without Visual Studio using VS Code & CMake
   - Full repository directory structure breakdown
   - How to extend Floweret (adding new scales, custom wavetables, and effects)
