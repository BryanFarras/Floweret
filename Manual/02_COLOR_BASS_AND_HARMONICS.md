# Floweret User Manual
# Chapter 2: Color Bass Engine & Harmonic Resonators

---

## 1. What is Color Bass?

**Color Bass** (also known as Melodic Riddim or Future Riddim) is a subgenre pioneered by artists such as *Chime*, *Ace Aura*, *Nasko*, and *Sharks*. 

In traditional dubstep, bass sounds are shaped using non-linear distortion, FM, and aggressive filtering, producing rich harmonic clusters across a wide frequency range. Color Bass takes these complex, aggressive timbres and passes them through **banks of tightly tuned resonant bandpass and comb filters** locked to the musical scale degrees of a song.

The result is a sound that retains the power and growl of dubstep, while simultaneously singing out in crystalline chords, arpeggios, and vocal-like formants.

---

## 2. The Color Bass Resonator Architecture

Floweret implements the scale-locked resonator architecture inspired by Xynth Audio's **Chroma** (`Misc/Documents/chroma web.pdf`):

```
  Input Audio (Wavetable / Growl / Grains / Noise)
       │
       ▼
 ┌─────────────────────────────────────────────────────────────┐
 │            SCALE-TUNED RESONANT FILTER BANK                 │
 │                                                             │
 │   Tuned Frequencies: f_i = 440 * 2^((MIDI_i - 69) / 12)     │
 │                                                             │
 │   Octave 2:  [Root]   [3rd]   [5th]   [7th]                 │
 │   Octave 3:  [Root]   [3rd]   [5th]   [7th]                 │
 │   Octave 4:  [Root]   [3rd]   [5th]   [7th]                 │
 │   Octave 5:  [Root]   [3rd]   [5th]   [7th]                 │
 └─────────────────────────────┬───────────────────────────────┘
                               │
                               ▼
 ┌─────────────────────────────────────────────────────────────┐
 │                COLOR OVERDRIVE (0 - 200%)                   │
 │   0 - 100%: Dry / Wet Mix                                   │
 │   100 - 200%: Q Multiplier + Cascade Hyper-Saturation      │
 └─────────────────────────────┬───────────────────────────────┘
                               │
                               ▼
 ┌─────────────────────────────────────────────────────────────┐
 │             SPECTRAL GATE & SPECTRAL MORPH                  │
 │   - Spectral Gate: Mutes off-key background bins            │
 │   - Spectral Morph: Preserves dry attack transients         │
 └─────────────────────────────┬───────────────────────────────┘
                               │
                               ▼
                         Tuned Output
```

---

## 3. Musical Scales & Key Selection

In the top generative toolbar, select your track's root note and scale:
- **Major**: `{ 0, 2, 4, 5, 7, 9, 11 }` (Uplifting, bright melodic color bass)
- **Natural Minor**: `{ 0, 2, 3, 5, 7, 8, 10 }` (Classic emotional melodic dubstep)
- **Harmonic Minor**: `{ 0, 2, 3, 5, 7, 8, 11 }` (Dark, theatrical, neoclassical)
- **Dorian**: `{ 0, 2, 3, 5, 7, 9, 10 }` (Lush, futuristic, jazzy)
- **Phrygian Dominant**: `{ 0, 1, 4, 5, 7, 8, 10 }` (Aggressive, exotic, quintessential heavy riddim)
- **Pentatonic Minor**: `{ 0, 3, 5, 7, 10 }` (Clean, open, punchy)
- **Pentatonic Major**: `{ 0, 2, 4, 7, 9 }` (Warm, ambient, botanica chords)
- **In-Sen**: `{ 0, 1, 5, 7, 10 }` (Traditional Japanese scale, mystical and ethereal)
- **Hirajoshi**: `{ 0, 2, 3, 7, 8 }` (Folk Japanese scale, introspective and resonant)

---

## 4. Understanding the Parameters

### `Color` (0.0 – 200.0)
The heart of the resonator:
- **`0.0 – 100.0`**: Functions as a transparent dry/wet crossfader between your dry oscillator mix and the resonant filter bank.
- **`100.0 – 200.0`**: Enters **Hyper-Resonance Overdrive**. The filter Q increases progressively by up to 3x, and an asymmetric saturation stage engages. This forces even purely noisy or inharmonic signals to ring with intense, crystalline musical formants.

### `Resonance` (0.1 – 30.0)
Sets the master sharpness (Q factor) of the resonant bandpass filters:
- **Low (1.0 – 4.0)**: Subtle tonal coloration and warm body.
- **Medium (5.0 – 12.0)**: Distinct, clear chords with good transient retention.
- **High (15.0 – 30.0)**: Laser-sharp ringing formants that transform any input into singing tones.

### `Spectral Morph` (0.00 – 1.00)
Blends the magnitude envelope of the dry signal with the wet tuned signal in the frequency domain. 
- At `0.00`, the wet resonator signal passes normally.
- At `1.00`, the dry signal's transient attack and punch are preserved while the harmonic body of the resonator rings out.

### `Spectral Gate` (-60.0 to 0.0 dB)
Acts as a dynamic threshold gate across all resonant bands:
- Frequencies falling below the threshold are silenced.
- This cleans up mud and removes discordant background rumble, ensuring only the strongest in-key notes are audible.

---

## 5. Sound Design Recipes

### Recipe 1: Signature Chime-Style Color Bass Lead
1. Set **`Wavetable Oscillator`** active:
   - Position: `0.35` (Saw/Triangle hybrid).
   - Warp Mode: `Phase Distortion`, Amount: `0.40`.
2. Set **`Key`** to `Eb`, **`Scale`** to `Major`.
3. Set **`Color`** to `135.0` (Hyper-resonance range).
4. Set **`Resonance`** to `12.0`.
5. Set **`Filter`** to `LowPass 24dB`, Cutoff: `5,500 Hz`.
6. Add **`Delay`** (`Mix: 0.20`, `Time: 1/8D`) and **`Reverb`** (`Mix: 0.30`, `Size: 0.65`).

### Recipe 2: Future Riddim Vocal Growl
1. Set **`Wavetable Oscillator`**:
   - Warp Mode: `Dual Warp`, Amount: `0.70`.
   - Modulate `WT Position` with `LFO 1` (Saw Down, Rate: `1/2` or `1/4`).
2. Set **`Noise Oscillator`** active:
   - Noise Color: `Violet Noise`, Level: `0.25` (adds high-frequency sizzle).
3. Set **`Key`** to `F`, **`Scale`** to `Phrygian Dominant`.
4. Turn **`Color`** to `160.0`.
5. Set **`Spectral Gate`** to `-28.0 dB` to eliminate between-growl mud.
6. Enable **`Sub Oscillator`** with `Sub Direct Out: ON` to keep the 40 Hz sub-bass bone-dry and centered.

### Recipe 3: Ethereal Botanica Resonant Bell
1. Set **`Multisample Oscillator`** active:
   - Bank: `Botanica Bells`.
2. Set **`Granular Oscillator`** active:
   - Position: `0.40`, Grain Size: `120 ms`, Density: `32`, Spray: `0.25`.
3. Set **`Key`** to `A`, **`Scale`** to `Hirajoshi (Japanese)`.
4. Turn **`Color`** to `75.0` and **`Bloom`** to `0.50`.
5. Enjoy shimmering, organic resonant cascades that evolve infinitely!
