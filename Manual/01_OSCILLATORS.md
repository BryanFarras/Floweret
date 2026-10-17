# Floweret User Manual
# Chapter 1: Oscillator Suite & Sound Engines

---

## 1. Wavetable Oscillator

The Wavetable Oscillator in Floweret is built to the modern high-definition standard:
- **Frame Size**: 2,048 samples per frame (industry standard as used in Serum, Vital, and modern hardware).
- **Table Capacity**: 64 morphed frames per table.
- **Interpolation**: Smooth Hermite (cubic) interpolation across both sample points and table frames for continuous, click-free position sweeping.

```
       Frame 0 (Sine)       Frame 21 (Triangle)    Frame 42 (Saw)      Frame 63 (Formant)
          /\                    /\                  /|                  /\  /\  /\
         /  \                  /  \                / |                 /  \/  \/  \
        /    \                /    \              /  |                /            \
       /      \              /      \            /   |               /              \
  ────/────────\────────────/────────\──────────/────|──────────────/────────────────\────
```

### Parameters
| Parameter | Range | Description |
| :--- | :--- | :--- |
| **Active** | On / Off | Enables or disables the wavetable engine. |
| **Position** | 0.00 – 1.00 | Continuous wavetable position scanning across all 64 frames. |
| **Warp Mode** | Menu | Selects the non-linear phase modifier applied to the waveform. |
| **Warp Amount** | 0.00 – 1.00 | Depth of the selected warp modifier. |
| **Smooth Interp** | On / Off | Toggles Hermite cubic vs. linear frame interpolation. |
| **Detune** | -24 to +24 st | Coarse and fine pitch tuning offset. |
| **Pan** | -1.0 to +1.0 | Stereo positioning. |
| **Level** | 0.00 – 1.00 | Output volume into the voice mixer. |

### Warp Modes Explained
- **Phase Distortion (PD)**: Casio CZ-style dual-slope phase transfer function. Warps the symmetry of the phase ramp, adding rich upper harmonics without changing the fundamental frequency.
- **Frequency Modulation (FM)**: Linear phase modulation. When the Sub Oscillator is active, it can serve as a clean low-frequency modulator for surgical growls and reese basses.
- **Ring Modulation (RM)**: Multiplies the wavetable audio by an internal carrier sine, producing sum and difference sideband frequencies for clangorous, metallic textures.
- **Hard Sync**: Emulates traditional analog oscillator hard sync by resetting the phase accumulator early based on the warp amount multiplier (up to 8x fundamental).
- **Bend (+) / Bend (-)**: Non-linearly compresses the waveform cycle towards the beginning or end of each period, producing hollow, vowel-like formant sweeps.
- **Dual Warp**: Combines both Phase Distortion and exponential Bend simultaneously for aggressive, modern dubstep timbres.

---

## 2. Granular Oscillator

The Granular Oscillator continuously transforms audio buffers into lush clouds of overlapping sonic micro-fragments ("grains").

```
  Audio Buffer Timeline:
  [───────────────────────|───────────────────────────────]
                         Scan Head
                           │
       ┌───────────┬───────┴───────┬───────────┐
     Grain 1     Grain 2         Grain 3     Grain 4
       /\          /\              /\          /\
      /  \        /  \            /  \        /  \
     (L 80%)     (R 20%)         (L 10%)     (R 90%)  <- Stereo Pan Spread
```

### Parameters
| Parameter | Range | Description |
| :--- | :--- | :--- |
| **Active** | On / Off | Enables or disables the granular cloud. |
| **Position** | 0.00 – 1.00 | Center scrub position within the audio buffer. |
| **Grain Size** | 10 – 500 ms | Duration of each individual grain. Smaller values sound buzzy/metallic; larger values preserve acoustic phrases. |
| **Density** | 2 – 64 grains/s | Number of overlapping grains spawned per second. High density creates dense ambient clouds. |
| **Spray** | 0.00 – 1.00 | Random spatial scatter around the center scrub position. |
| **Pitch Jitter** | 0 – 12 st | Random pitch transposition range applied to each newly spawned grain. |
| **Freeze** | On / Off | Halts buffer playhead advance while grain generation continues, capturing an eternal sustained pad. |
| **Level** | 0.00 – 1.00 | Output volume into the voice mixer. |

---

## 3. Spectral Oscillator

The Spectral Oscillator performs additive harmonic resynthesis of audio material at the partial level, breaking audio down into 64 discrete sinusoidal components.

```
  Harmonics: 1   2   3   4   5   6   7   8   9   10  11  ...  64
  Raw:       █   ▆   ▄   ▃   ▂   ▂   -   -   -   -   -
  Quantized: █       ▄       ▂               -                <- Locked to Active Scale!
```

### Key Capabilities
- **Harmonic Scale Quantization (`Harmonic Quantize`)**: 
  When enabled, partial frequencies that deviate from the musical scale degrees (selected in the master toolbar) are pulled towards the nearest scale note. This ensures that even complex atonal or noisy source material snaps cleanly into harmonious Color Bass chords.
- **Spectral Tilt**: Tilts the harmonic slope across the partial series. Positive values boost high partials for crystalline brightness; negative values emphasize the fundamental and warm lower partials.
- **Spectral Smear**: Blurs amplitude and frequency transitions over time, turning sharp transients into smooth, glistening glass textures.

---

## 4. Sample Oscillator

Dedicated high-precision sample playback engine featuring:
- **Drag & Drop Sample Loading**: Drag any `.wav`, `.mp3`, `.flac`, `.aiff`, or `.ogg` file directly from Windows Explorer, FL Studio's Browser, or your desktop into the **ONE-SHOT FILES** drop zone (or click the **ADD** button on the toolbar). Floweret will automatically decode, normalize, and activate the sample across all polyphonic voices.
- **Snap-to-Zero Crossing**: Automatically analyzes sample start and loop points, snapping them to exact zero-crossing phase points to prevent clicks and pops.
- **Loop Modes**:
  - `One-Shot`: Plays through once from start to finish.
  - `Forward Loop`: Continuously loops between `Loop Start` and `Loop End`.
  - `Ping-Pong`: Smoothly alternates forward and reverse playback between loop markers.
- **Tape-Stop Inertia**: Variable rate deceleration curve. When automated, it produces realistic vinyl brake, motor slowdown, and glitch pitch-drop effects.
- **Transient Slicing**: Automatically segments audio into rhythmic beat slices selectable via `Slice Index`.

---

## 5. Multisample Oscillator

Designed for realistic reproduction of organic acoustic instruments:
- **Multi-Zone Mapping**: Automatically routes incoming MIDI notes and velocities to appropriate high-resolution recordings.
- **SFZ Architecture**: Supports custom `.sfz` multi-sample descriptor files.
- **Factory Botanical Banks**:
  1. **Botanica Bells**: Tuned crystal and bell strikes with long organic resonance.
  2. **Organic Kalimba**: Wooden tines with acoustic body impulse resonance.
  3. **Pizzicato Flora**: Delicate plucked botanical strings.
  4. **Ethereal Flute**: Breath transients and overblown harmonic overtones.

---

## 6. Colored Noise Oscillator

Based on acoustic physics and psychoacoustics (referencing `noise colors.pdf`), Floweret includes a full spectrum of noise generators:

```
  +6 dB/oct ─── Violet Noise
  +3 dB/oct ─── Blue Noise
   0 dB/oct ─── White Noise (flat power density)
  -3 dB/oct ─── Pink Noise (equal energy per octave, 1/f)
  -6 dB/oct ─── Brown Noise (integrated Brownian motion, 1/f²)
  Custom    ─── Grey Noise (inverted A-weighting equal loudness)
  Impulses  ─── Velvet Noise (sparse random pulses for lush diffusion)
```

### Noise Types
1. **White**: Flat frequency spectrum across all audible bands. Ideal for bright percussive attacks and snares.
2. **Pink**: Power drops at -3 dB/octave (1/f). Matches the frequency response of human hearing; standard for organic acoustic emulation.
3. **Brown**: Power drops at -6 dB/octave (1/f²). Warm, rumbling, thunderous low-end roar; perfect for deep atmospheric beds.
4. **Blue**: Power rises at +3 dB/octave (+10 dB/decade). High-frequency shimmer with no low-end clutter.
5. **Violet**: Power rises at +6 dB/octave (+20 dB/decade). Intense high-frequency hiss, ideal for air band sweetening and crisp transients.
6. **Grey**: Filtered with an inverted A-weighting psychoacoustic equal-loudness curve. Perceived as equally loud at all human hearing frequencies.
7. **Velvet Noise**: A sparse sequence of random positive and negative impulses (taps/second). Completely free of harsh phase buildup, it creates lush, silky decorrelated air and organic ambient textures.

---

## 7. Sub Oscillator

The foundation of modern Dubstep, Drum & Bass, and Color Bass:
- **Waveforms**: Pure Sine, Triangle, Square.
- **Octave Offsets**: Selectable `-1 Oct`, `-2 Oct`, or `-3 Oct` below the played pitch.
- **Saturation Drive**: Applies soft hyperbolic tangent saturation to add warm second and third harmonics so the sub translates onto smaller speakers and mobile phones.
- **Sub Direct Routing**: When enabled, the Sub Oscillator bypasses the voice filter and stereo effects rack entirely. It is routed directly to the master output in clean mono, ensuring your sub-bass never suffers from phase cancellation or stereo mud.
