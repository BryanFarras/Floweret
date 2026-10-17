# Floweret User Manual
# Chapter 4: Filters, Effects Rack & Signal Routing

---

## 1. Master Signal Flow Diagram

Floweret’s signal architecture is designed for maximum sonic cleanliness, separating delicate high frequencies and stereo spatialization from mono low-end weight:

```
  ┌─────────────────────────────────────────────────────────────┐
  │                     OSCILLATORS MIXER                       │
  │  [Wavetable]  [Granular]  [Spectral]  [Sample]  [Multi]  [Noise]
  └──────────────────────────────┬──────────────────────────────┘
                                 │
                                 ▼
  ┌─────────────────────────────────────────────────────────────┐
  │                 VOICE MULTI-MODE FILTER                     │
  │     LP12/24, HP12/24, BP, Notch, Comb, Formant / Vowel      │
  └──────────────────────────────┬──────────────────────────────┘
                                 │
                                 ▼
  ┌─────────────────────────────────────────────────────────────┐
  │                 COLOR BASS RESONATOR BANK                   │
  │           Scale-Tuned Resonant Filter Matrix                │
  └──────────────────────────────┬──────────────────────────────┘
                                 │
                                 ▼
  ┌─────────────────────────────────────────────────────────────┐
  │                      EFFECTS RACK                           │
  │    Distortion / Wavefolder ──► Ping-Pong Delay ──► Reverb   │
  └──────────────────────────────┬──────────────────────────────┘
                                 │
                                 ▼
                      ┌──────────────────────┐
                      │    SUMMING STAGE     │◄───── [SUB OSCILLATOR]
                      └──────────┬───────────┘       (Direct Out bypasses
                                 │                    Filter, Resonator,
                                 ▼                    and Stereo FX!)
                        [MASTER OUTPUT]
```

---

## 2. Multi-Mode Voice Filter

The voice filter is a zero-delay feedback State Variable Filter (SVF) with dedicated analog saturation drive:

```
  Low-Pass (LP):         High-Pass (HP):        Band-Pass (BP):        Notch:
  ───────\               /───────               /\                     ───\  /───
          \             /                      /  \                        \/
           \───     ───/                      /    \
```

### Filter Types
1. **LowPass 12dB**: Gentle 2-pole rolloff. Preserves natural high-end air while softening brightness.
2. **LowPass 24dB**: Steep 4-pole rolloff. The classic subtractive workhorse for aggressive bass plucks and laser sweeps.
3. **HighPass 12dB & 24dB**: Removes muddy low frequencies; ideal when layering a sound over an independent sub-bass.
4. **BandPass**: Isolates mid-range frequency bands for "telephone" and focused vocal tones.
5. **Notch**: Carves out a surgical notch frequency; excellent for flanger-like sweeps when modulated by an LFO.
6. **Comb Filter**: Incorporates a short, tuned delay line with positive feedback. Creates metallic, robotic, and Karplus-Strong string resonances.
7. **Formant / Vowel**: Dual tuned bandpass filters simulating vocal tract formants (`/a/`, `/e/`, `/o/`). Sweeping cutoff produces expressive "wah-wah" speech timbres.

---

## 3. Waveshaping Distortion Processor

Floweret includes 5 distinct distortion algorithms tailored for heavy bass and glitch textures:

```
  Soft Saturate:         Hard Clip:             Foldback (Glitch):     Bitcrush:
       _                      ┌────                  /\   /\           ┌──┐
     /                      /                      /  \ /  \           │  └──┐
   /                      /                      /    V    \         ──┘     └──
```

1. **Soft Saturate**: Hyperbolic tangent saturation curve. Warms signals with smooth odd harmonics; safe for master busses.
2. **Hard Clip**: Hard digital brickwall clipping at 0 dB. Produces aggressive, buzzy square-wave overtones essential for modern tearout and dubstep.
3. **Foldback (Wavefolder)**: When the waveform amplitude exceeds 1.0, the peaks reflect backward instead of clipping. This creates complex inharmonic sidebands, metallic shrieks, and glitch textures.
4. **Bitcrush / Decimate**: Reduces digital resolution to between 4 and 16 bits. Adds gritty 8-bit crunch, lo-fi aliasing, and vintage video-game character.
5. **Tube Drive**: Asymmetric vacuum tube transfer function that introduces musical second-order even harmonics for punchy warmth.

---

## 4. Stereo Ping-Pong Delay

- **`Delay Time` (0.01 – 2.00 s)**: Sets the delay length between left and right echoes.
- **`Feedback` (0.00 – 0.95)**: Number of repeats. Cross-feedback topology bounces repeats from Left to Right and back.
- **`Mix` (0.00 – 1.00)**: Dry/wet blend.

---

## 5. Algorithmic Diffusion Reverb ("Botanica Bloom")

The reverb engine uses an 8-comb filter Schroeder/Moorer matrix paired with stereo allpass diffusers to generate deep, organic spaces:
- **`Size` (0.00 – 1.00)**: Expands the virtual room volume from a tight resonant wooden chamber up to a massive cathedral bloom.
- **`Damping` (0.00 – 1.00)**: Simulates atmospheric high-frequency absorption. Higher values make the reverb tail darker and warmer.
- **`Mix` (0.00 – 1.00)**: Dry/wet balance. Controlled by the master **`BLOOM`** and **`TAIL`** knobs on the Growth Field.

---

## 6. Sub-Bass Direct Out Routing

In heavy electronic genres like Color Bass and Dubstep, complex stereo distortion, chorus, delay, and reverb can cause phase cancellation and muddy the low-end.

To solve this:
- Turn **`Sub Direct Out`** **ON** in the Sub Oscillator.
- The sub-bass signal completely bypasses all filters, Color resonators, distortions, delays, and reverbs.
- It is summed directly into the master output in clean, mono alignment.
- You can apply extreme pitch bends, distortion, and spatial reverb to your mid-range growls while your 40 Hz sub foundation remains solid and punchy.
