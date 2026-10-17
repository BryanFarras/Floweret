# Floweret User Manual
# Chapter 3: Modulation, Arpeggiator & Clip Sequencer

---

## 1. Envelope Generators

Floweret features sample-accurate multi-stage envelope generators for sculpting amplitude and filter cutoff dynamics:

```
    Level
      1.0 ┌       ▲
          │      / \
          │     /   \
  Sustain ├─── /     \─────────────────────────────┐
          │   /  Decay                             │
          │  / Attack                              │ Release
      0.0 └──┴─────────────────────────────────────┴─────────► Time
             Note On                               Note Off
```

### Amplitude Envelope (`ENV 1`)
Controls the volume trajectory of each polyphonic voice:
- **`Attack` (1 ms – 10 s)**:
  - `1 – 15 ms`: Punchy, percussive, stabby attacks with zero latency or click.
  - `20 – 100 ms`: Smooth acoustic instrument pluck.
  - `200 ms – 5 s`: Swelling ambient pads and generative blooms.
- **`Decay` (10 ms – 10 s)**: Time required to fall from peak attack level to the sustain level.
- **`Sustain` (0.00 – 1.00)**: Level held while a key is sustained.
- **`Release` (10 ms – 10 s)**: Time required for the sound to fade to silence after key release.

### Filter Envelope (`ENV 2`)
Dedicated modulation envelope hardwired to the Multi-Mode Filter:
- **`Filter Env Amount` (-1.00 to +1.00)**: Determines how far the filter cutoff opens or closes during the envelope cycle. Negative values invert the envelope for reverse-sweeping suction effects.

---

## 2. Low Frequency Oscillators (LFOs)

Floweret’s LFO engine provides rhythmic and chaotic movement:

```
  Sine:         Triangle:       Saw Down:       Square:         Random S&H:
     /\            /\           |\   |\         ┌─┐ ┌─┐         ┌─┐
    /  \          /  \          | \  | \        │ │ │ │         │ │ ┌───┐
  ──    ──      ──    ──      ──|──\─|──\─    ──┘ └─┘ └──     ──┘ └─┘   └─
```

### Parameters
- **`LFO Rate` (0.05 Hz – 30.0 Hz)**: Sets LFO speed, capable of both slow evolving generative drifts and fast audio-rate modulations.
- **`LFO Shape`**:
  - `Sine`: Smooth, rounded oscillation.
  - `Triangle`: Linear rise and fall; ideal for filter sweeps.
  - `Saw Down`: Percussive, repeating drops; standard for dubstep wobbles.
  - `Saw Up`: Rising ramp modulation.
  - `Square`: Rhythmic binary on/off switching.
  - `Random S&H`: Stepped random voltages triggered at each cycle.
- **`Chaos / Smooth Drift` (0.00 – 1.00)**: 
  Introduces Brownian random motion into the LFO waveform. As Chaos increases, the periodic waveform begins to drift unpredictably like natural wind or flowing water — a core technique for living, breathing **Botanica** textures.

---

## 3. The Arpeggiator

The built-in arpeggiator takes held chords and breaks them into musical melodic runs:

```
  Input: C - E - G (Chord)
  Up:      C3 ──► E3 ──► G3 ──► C4 ──► E4 ──► G4
  Down:    G4 ──► E4 ──► C4 ──► G3 ──► E3 ──► C3
  Up/Down: C3 ──► E3 ──► G3 ──► G4 ──► E4 ──► C3
  Random:  E3 ──► C4 ──► G3 ──► C3 ──► G4 ──► E4
```

### Parameters
- **`Active`**: Toggles the arpeggiator on or off.
- **`Pattern Mode`**:
  - `Up`: Plays notes from lowest to highest.
  - `Down`: Plays notes from highest to lowest.
  - `Up/Down`: Cycles up then back down smoothly.
  - `Random`: Randomly triggers notes from the active chord pool.
  - `Chord`: Rhythmically gates the full chord simultaneously.
- **`Rate`**: Tempo-synced clock divisions: `1/4`, `1/8`, `1/16`, `1/32`, `1/8T` (triplet), `1/16T` (triplet).
- **`Octaves` (1 – 4)**: Duplicates the chord notes across up to 4 octaves.

---

## 4. The 16-Step Clip Sequencer

For rhythmic sequences, melodic hooks, and glitch fills:

```
  Step:    01  02  03  04  05  06  07  08  09  10  11  12  13  14  15  16
  Active:  [x] [ ] [x] [ ] [x] [ ] [x] [x] [x] [ ] [x] [ ] [x] [ ] [x] [x]
  Offset:  +0      +3      +7      +10 +12 +0      +5      +7      +12 +15
  Glitch:  ────────────────────────────► [Random Octave Jump / Ratchet!]
```

### Key Capabilities
- **Per-Step Controls**: Each of the 16 steps has an independent `Active` toggle, `Semitone Offset` (-12 to +12 st), `Velocity`, and `Gate Length`.
- **Glitch Probability (`seq_glitch_prob`)**:
  - Dialed from `0%` to `100%`.
  - At `0%`, the sequence plays strictly as programmed.
  - As probability is increased, steps have a chance to dynamically ratchet, jump octaves, or mutate pitch on the fly, creating organic glitch variations that never sound repetitive.
