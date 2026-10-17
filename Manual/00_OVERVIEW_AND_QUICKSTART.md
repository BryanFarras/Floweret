# Floweret User Manual

# Chapter 0: Overview & Quickstart Guide

---

## 1. Introduction to Floweret

**Floweret** is a next-generation hybrid synthesizer designed specifically for modern electronic music production with a focus on:

- **Color Bass** (melodic, scale-quantized resonator basses and resonant vocal growls)
- **Botanica** (organic acoustic textures, delicate ambient diffusions, and generative swells)
- **Dubstep** (heavy, surgical wavetable basses with pristine mono sub-bass)
- **Glitch & IDM** (granular deconstruction, wavefolding, ratcheting sequencers, and tape-stop dynamics)

Inspired by the workflow depth of Serum and Vital, Floweret pushes sound design further by uniting **Wavetable**, **Granular Cloud**, **Spectral Resynthesis**, **Sample Slicing**, **Multisample Instruments**, **Colored Noise**, and **Scale-Locked Resonators** inside a unified organic interface.

---

## 2. The Grassy Design Aesthetic

Floweret’s user interface is built on the **GRASS** design language:

- **Ivory / Cream Canvas**: An open, warm visual environment that eliminates eye fatigue during prolonged sound design sessions.
- **Deep Moss & Pine Accents**: High-contrast, elegant typography and structural lines.
- **Frosted Glass Panels**: Translucent glassmorphism with subtle drop shadows and soft edge highlights.
- **The Growth Field**: A central animated canvas where floating translucent cards and organic terrain curves react dynamically to audio energy, voice density, and macro adjustments.
- **Rotary Dials**: Frosted circular knobs with glowing perimeter needle arcs and precision numeric readouts.

---

## 3. Interface Tour

```
┌────────────────────────────────────────────────────────────────────────┐
│  FLOWERET   [ Quick Guide Banner ]          [Volume: 0.0dB]  [0 VOICES]│
├────────────────────────────────────────────────────────────────────────┤
│  A GENERATIVE BOTANICAL INSTRUMENT   [ADD] [CLEAR] [RESET] [SPROUT]    │
│  [ ONE-SHOT DROP ]        [ LOOP & SFZ DROP ]    [KEY: Eb] [SCALE: Maj]│
├────────────────────────────────────────────────────────────────────────┤
│  [GROWTH FIELD] [WAVETABLE] [GRANULAR] [SPECTRAL] [FX & COLOR] [SEQ]   │
│ ┌────────────────────────────────────────────────────────────────────┐ │
│ │                                                                    │ │
│ │                    CENTRAL WORKSPACE CANVAS                        │ │
│ │             (Growth Field / Oscillators / FX / Sequencer)          │ │
│ │                                                                    │ │
│ └────────────────────────────────────────────────────────────────────┘ │
├────────────────────────────────────────────────────────────────────────┤
│  [C3]  [C#]  [D]  [D#]  [E]  [F]  ... [C7]   VIRTUAL KEYBOARD          │
└────────────────────────────────────────────────────────────────────────┘
```

### Top Header Bar

- **Logo**: Click to access global preferences and plugin information.
- **Guide Banner**: Displays contextual parameter descriptions when hovering over controls.
- **Output Volume Slider**: Master output gain fader with real-time decibel readout.
- **Voice & Sample Counter**: Displays current polyphonic voice allocation and loaded sample banks.
- **Theme Toggle**: Switch between **Light Botanica** and **Dark Forest** themes.

### Generative Action Strip

- **`ADD`**: Quick-adds a new audio layer or sample file to the active slot.
- **`CLEAR`**: Resets the current oscillator's sample buffer.
- **`RESET`**: Restores the default initialization patch.
- **`SPROUT`**: The signature generative mutation button. Randomizes and musically offsets macro parameters, wavetable positions, and grain spray while preserving harmonic coherence.
- **`KEY` & `SCALE` Dropdowns**: Master musical scale selectors. Directly controls the harmonic resonance bank in the Color Bass engine.

### Central Workspace Switcher

Navigate between specialized workspaces:

1. **`GROWTH FIELD`**: The primary macro performance canvas with 5 master generative dials (`COLOR`, `MUTATE`, `BLOOM`, `SPREAD`, `TAIL`).
2. **`WAVETABLE`**: 2D/3D morphing wavetable viewer with real-time warp distortion controls.
3. **`GRANULAR`**: Live particle grain cloud display with spray, size, and freeze controls.
4. **`SPECTRAL`**: Chroma-style harmonic bar spectrum with dry vs. tuned resonance peaks.
5. **`FX & COLOR`**: Resonator filter bank, multi-mode SVF, distortion waveshaper, delay, and reverb.
6. **`SEQUENCER`**: 16-step clip sequencer and multi-pattern arpeggiator.

### Bottom Virtual Keyboard

- Spans 4 full octaves (C3 to C7).
- Deep pine green black keys and ivory white keys.
- Click or drag with the mouse to audition notes and test pitch response directly without requiring a MIDI keyboard.

---

## 4. Quickstart: Your First Sound

### Step 1: Initialize the Patch

1. Launch **Floweret** (either inside your DAW or via the Standalone application).
2. Click **`RESET`** in the top toolbar to load the clean initial patch.

### Step 2: Set the Musical Key

1. In the top toolbar, set **`KEY`** to `Eb` and **`SCALE`** to `Major` (or `Phrygian Dominant` for heavy dubstep).
2. Notice how all harmonic quantizers and the Color Bass resonator instantly align to your track's key.

### Step 3: Dial in the Color Resonator

1. Switch to the **`GROWTH FIELD`** tab.
2. Turn the **`COLOR`** knob to `30.0`:
   - Below `100.0`, the Color dial blends in resonant harmonics matching your chosen scale.
   - Turn **`COLOR`** past `100.0` (up to `200.0`): the resonator increases filter resonance (Q) and drives the signal into singing vocal formants.

### Step 4: Explore the Granular Cloud

1. Click the **`GRANULAR`** tab.
2. Ensure **Granular Active** is turned on.
3. Adjust **`Grain Size`** (e.g., 60 ms) and **`Density`** (e.g., 24 grains/sec).
4. Scrub **`Position`** to glide through the organic soundscape buffer.
5. Toggle **`Freeze`** to capture a continuous atmospheric pad.

### Step 5: Press `SPROUT`

Click the **`SPROUT`** button in the toolbar. Watch the dials, grain distribution, and wavetable position blossom into a new generative soundscape!
