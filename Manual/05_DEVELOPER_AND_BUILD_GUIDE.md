# Floweret User Manual

# Chapter 5: Developer & Build Guide (VS Code & CMake)

---

## 1. Building Without Visual Studio

Floweret is completely configured to build on Windows using **VS Code and CMake** without needing the heavy Visual Studio IDE.

### Prerequisites

- **Visual Studio Code** (installed at `C:\Users\asus\AppData\Local\Programs\Microsoft VS Code\Code.exe`)
- **CMake** (installed at `C:\Program Files\CMake\bin\cmake.exe`)
- **JUCE 8** (located at `C:\Users\asus\Documents\JUCE`)
- **C/C++ Compiler**: A C++17 compliant compiler (MSVC Build Tools, Clang-cl, or Clang).

---

## 2. Quick Build in VS Code

1. Open the project root in VS Code:

   ```powershell
   code "c:\Users\asus\Documents\Juce Projects\Floweret"
   ```

2. **Select Compiler Kit**:
   When prompted by the **CMake Tools** status bar at the bottom, select your preferred compiler kit.

3. **Build the Project**:
   Press **`Ctrl + Shift + B`** (or click **Build** in the bottom status bar).

   VS Code will invoke CMake:

   ```powershell
   cmake -B build
   cmake --build build --config Release
   ```

4. **Output Targets**:
   - **VST3 Plugin**: `build/Floweret_artefacts/Release/VST3/Floweret.vst3`
   - **Standalone App**: `build/Floweret_artefacts/Release/Standalone/Floweret.exe`

5. **Run the Standalone Synth**:
   Press **`F5`** or execute the task **"Run Standalone Plugin"** via `Ctrl + Shift + P` -> `Tasks: Run Task`.

---

## 3. Codebase Directory Structure

```
Floweret/
├── CMakeLists.txt                 <- Master CMake configuration file
├── Floweret.jucer                 <- Projucer project definition file
├── README.md                      <- Project overview & repository readme
├── .gitignore                     <- Git rules (ignores build artifacts & Misc)
│
├── .vscode/                       <- VS Code IDE integration
│   ├── settings.json              <- CMake and C++ paths
│   ├── c_cpp_properties.json      <- IntelliSense include paths & JUCE defines
│   ├── tasks.json                 <- Build & run tasks (Ctrl+Shift+B)
│   ├── launch.json                <- Debugging standalone target (F5)
│   └── extensions.json            <- Recommended extensions
│
├── Manual/                        <- Comprehensive user & developer manual
│   ├── INDEX.md                   <- Documentation table of contents
│   ├── 00_OVERVIEW_AND_QUICKSTART.md
│   ├── 01_OSCILLATORS.md
│   ├── 02_COLOR_BASS_AND_HARMONICS.md
│   ├── 03_MODULATION_AND_SEQUENCING.md
│   ├── 04_EFFECTS_AND_ROUTING.md
│   └── 05_DEVELOPER_AND_BUILD_GUIDE.md
│
├── Misc/                          <- Local inspiration & research documents
│   ├── Documents/                 <- PDF research on Chroma, Noise colors, Wavetables
│   └── Inspo/                     <- Visual UI mockup (GRASS)
│
└── Source/                        <- C++ DSP & UI Source Code
    ├── PluginProcessor.h / .cpp   <- AudioProcessor, APVTS, State serialization
    ├── PluginEditor.h / .cpp      <- AudioProcessorEditor, layout, parameter attachments
    │
    ├── Common/
    │   ├── Parameters.h           <- APVTS Parameter IDs, layouts, musical scales
    │   └── AudioHelpers.h         <- Hermite interpolation, fast math, conversions
    │
    ├── DSP/
    │   ├── FloweretSynth.h / .cpp <- Master polyphonic synth engine & voice allocator
    │   ├── FloweretVoice.h / .cpp <- Polyphonic voice combining all oscillators & envelopes
    │   │
    │   ├── Oscillators/
    │   │   ├── WavetableOscillator.h / .cpp   <- 2048-sample morphing wavetable
    │   │   ├── GranularOscillator.h / .cpp    <- 64-grain dynamic grain cloud
    │   │   ├── SpectralOscillator.h / .cpp    <- Additive harmonic resynthesis
    │   │   ├── SampleOscillator.h / .cpp      <- Snap-to-zero sample playback & tape-stop
    │   │   ├── MultisampleOscillator.h / .cpp <- Multi-zone acoustic instrument engine
    │   │   ├── NoiseOscillator.h / .cpp       <- Colored noise generators
    │   │   └── SubOscillator.h / .cpp         <- Sub-bass oscillator with drive & direct out
    │   │
    │   ├── FX/
    │   │   ├── ColorResonator.h / .cpp        <- Chroma-style scale-tuned resonator bank
    │   │   ├── MultiModeFilter.h / .cpp       <- SVF LP/HP/BP/Notch/Comb/Formant filter
    │   │   └── EffectsRack.h / .cpp           <- Waveshaper, Delay, and Reverb
    │   │
    │   ├── Sequencer/
    │   │   ├── Arpeggiator.h                  <- Multi-mode BPM-synced arpeggiator
    │   │   └── ClipSequencer.h                <- 16-step clip sequencer with glitch probability
    │   │
    │   └── Modulation/
    │       ├── EnvelopeGenerator.h            <- Multi-stage ADSR envelope
    │       └── LFO.h                          <- Multi-shape LFO with chaos drift
    │
    └── UI/
        ├── BotanicaLookAndFeel.h / .cpp       <- GRASS aesthetic custom controls & dials
        └── Components/
            ├── GrowthFieldVisualizer.h / .cpp <- Real-time floating frosted card visualizer
            ├── BotanicaKnob.h                 <- Frosted rotary dial with title and badge
            ├── WavetableDisplay.h             <- 2D/3D morphing wavetable viewer
            ├── GrainCloudDisplay.h            <- Dynamic grain particle cloud visualizer
            ├── SpectralVisualizer.h           <- Real-time Chroma harmonic bar spectrum
            └── ResonatorKeyboard.h / .cpp     <- Bottom virtual piano keyboard
```

---

## 4. How to Extend Floweret

### Adding a New Musical Scale

Open [`Source/Common/Parameters.h`](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Source/Common/Parameters.h):

1. Add your scale enum to `enum class MusicalScale`.
2. Add its display name to `getScaleNames()`.
3. Add its semitone intervals to `getScaleIntervals()`.
   The Color Bass resonator and spectral quantizer will automatically adapt!

### Adding Custom Wavetable Frames

Open [`Source/DSP/Oscillators/WavetableOscillator.cpp`](file:///c:/Users/asus/Documents/Juce%20Projects/Floweret/Source/DSP/Oscillators/WavetableOscillator.cpp):
In `generateFactoryWavetables()`, write additive equations or load `.wav` files into the `frames[f]` vectors.
Each frame is 2,048 float samples normalized between `-1.0f` and `+1.0f`.
