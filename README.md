![Floweret Logo](Assets/Logo/floweret_logo_transparent.png)

> **Botanica Granular & Wavetable Synthesizer**  
> Focused on _Color Bass_, _Botanica_, _Dubstep_, and _Glitch_.  
> Built with JUCE 8 (VST3 & Standalone).

> Development starts at October 17, 2026.  
> Expected release date at May 16, 2027.

---

## Overview & Aesthetic

**Floweret** combines the wavetable power of modern synths like Serum and Vital with an organic granular synthesis engine, real-time spectral resynthesis, and scale-locked harmonic resonators tailored for **Color Bass** and **Botanica** music.

The visual design is inspired by the serene, organic **GRASS** aesthetic:

- **Palette**: Warm ivory / soft cream canvas, deep pine / moss green accents, and glowing sage pointers.
- **Growth Field**: Animated real-time visualizer featuring floating translucent frosted glass panels over generative terrain curves.
- **Botanica LookAndFeel**: Custom frosted rotary dials with perimeter meters, numeric badges, organic pill buttons, and responsive piano keyboard.

---

## Oscillator Types & Sound Engines

### 1. Wavetable Oscillator (`Source/DSP/Oscillators/WavetableOscillator.h`)

- **2048-sample multi-frame wavetables** (64 frames per table) matching modern industry standards.
- **Smooth Interpolation Mode**: Hermite cubic interpolation for continuous, near-infinite frame positions.
- **Warp Modes**:
  - **Phase Distortion (PD)**: Casio CZ-style dual-slope phase transfer function.
  - **Frequency Modulation (FM)**: Linear phase modulation driven by internal modulators or Sub-oscillator.
  - **Ring Modulation (RM)**: Pure harmonic multiplication.
  - **Hard Sync**: Virtual slave oscillator sync.
  - **Bend (+) / Bend (-)**: Non-linear cycle compression.
  - **Dual Warp**: Combined Phase Distortion and exponential Bend.

### 2. Granular Oscillator (`Source/DSP/Oscillators/GranularOscillator.h`)

- **Dynamic Grain Cloud Engine** with up to 64 overlapping grains.
- **Controls**:
  - `Position`: Scrub anywhere in the audio buffer.
  - `Grain Size`: 10 ms to 500 ms with Hann windowing.
  - `Density`: 2 to 64 grains per second.
  - `Spray`: Spatial position scatter/jitter.
  - `Pitch Jitter`: Random micro-tuning / semitone drift per grain.
  - `Freeze`: Locks scan head while the grain cloud sustains evolving organic textures.
  - `Stereo Spread`: True stereo random panning per grain.

### 3. Spectral Oscillator (`Source/DSP/Oscillators/SpectralOscillator.h`)

- **Harmonic Level Resynthesis**: 64 sinusoidal partials synthesized in real time.
- **Color Bass Harmonic Quantization**: Snaps partial frequencies to the exact musical notes of the selected scale (Major, Minor, Phrygian Dominant, Pentatonic, In-Sen, Hirajoshi).
- **Spectral Tilt & Smear**: Darkens/brightens harmonic slopes and blurs spectral frames for frozen metallic crystalline pads.

### 4. Sample Oscillator (`Source/DSP/Oscillators/SampleOscillator.h`)

- Sample playback engine with **snap-to-zero loop detection**.
- **Loop Modes**: One-Shot, Forward Loop, Ping-Pong Loop.
- **Tape-Stop Inertia**: Variable rate deceleration for vinyl brake and glitch pitch-drops.
- **Transient Slicing**: Real-time score slice point extraction.

### 5. Multisample Oscillator (`Source/DSP/Oscillators/MultisampleOscillator.h`)

- Multi-velocity and multi-key zone mapping engine.
- Includes factory botanical acoustic instrument banks:
  - _Botanica Bells_, _Organic Kalimba_, _Pizzicato Flora_, _Ethereal Flute_.

### 6. Colored Noise Oscillator (`Source/DSP/Oscillators/NoiseOscillator.h`)

- Full research-backed noise palette (from `noise colors.pdf`):
  - **White**: Uniform flat spectrum.
  - **Pink**: -3 dB/octave (1/f) Paul Kellet filter cascade.
  - **Brown**: -6 dB/octave (1/f²) leaky integration.
  - **Blue**: +3 dB/octave (f) differentiated noise.
  - **Violet**: +6 dB/octave (f²) second-order difference.
  - **Grey**: Psychoacoustic inverted equal-loudness curve.
  - **Velvet**: Sparse random positive/negative impulses for smooth, decorrelated air.

### 7. Sub Oscillator (`Source/DSP/Oscillators/SubOscillator.h`)

- Clean sub-bass foundation with -1 / -2 / -3 Octave offsets.
- Saturation drive with soft hyperbolic clipping.
- **Sub Direct Routing**: Option to route sub directly to master output, bypassing heavy filters and stereo FX to maintain an immovable mono low-end punch for Dubstep and Color Bass.

---

## Color Bass Resonator (`Source/DSP/FX/ColorResonator.h`)

Inspired by Xynth Audio's **Chroma** (`Misc/Documents/chroma web.pdf`):

- **Scale-Tuned Resonator Bank**: Biquad bandpass filters tuned to musical scale degrees across 6 octaves (100 Hz to 6 kHz).
- **Musical Scales**: Major, Natural Minor, Harmonic Minor, Dorian, Phrygian Dominant, Pentatonic Minor/Major, In-Sen, Hirajoshi.
- **Color Knob (0 to 200%)**:
  - `0 - 100%`: Dry/wet blend into the resonant spectrum.
  - `100 - 200%`: Boosts filter Q, introduces cascade saturation, forcing harsh growls, grains, and noise to sing in musical chords.
- **Spectral Visualizer**: Real-time display showing DRY (purple) vs. TUNED (cyan/sage) harmonic activity.

---

## Effects Rack & Modulation

- **Effects Rack**:
  - Multi-algorithm Waveshaper: Soft Saturate, Hard Clip, Foldback (Glitch wavefolder), Bitcrusher, Tube Drive.
  - Stereo Ping-Pong Delay with cross-feedback and tempo sync.
  - Algorithmic Reverb: Schroeder/Moorer comb & allpass diffusion network.
  - Multi-Mode SVF Filter: LP12/24, HP12/24, BandPass, Notch, Comb, and Formant/Vowel filters.
- **Modulation**:
  - Multi-stage ADSR Envelopes with exponential curvature.
  - LFOs with Sine, Triangle, Saw Up/Down, Square, Random S&H, and Smooth Chaos drift.
- **Performance**:
  - Multi-mode Arpeggiator (Up, Down, Up/Down, Random, Chord).
  - 16-step Clip Sequencer with pitch offsets, velocity, gate, and Glitch probability.

---

## Building the Project with VS Code

### Using VS Code & CMake (Recommended)

1. Open this project directory in **VS Code**:
   ```bash
   code .
   ```
2. When prompted by the **CMake Tools** extension, select your compiler kit.
3. Click **Build** in the bottom status bar, or press `Ctrl + Shift + B`.
4. The generated targets:
   - **VST3 Plugin**: `build/Floweret_artefacts/Release/VST3/Floweret.vst3`
   - **Standalone App**: `build/Floweret_artefacts/Release/Standalone/Floweret.exe`
5. Press `F5` or run the task **"Run Standalone Plugin"** from the Command Palette (`Ctrl + Shift + P` -> `Tasks: Run Task`) to launch and test the synth immediately.

### Alternative: Command Line (CMake)

```bash
cmake -B build
cmake --build build --config Release
```
