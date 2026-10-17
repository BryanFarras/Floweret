# Floweret User Manual

# Chapter 5: Developer & Build Guide (VS Code & CMake)

---

## 1. Building Without Visual Studio

Floweret is completely configured to build on Windows using **VS Code and CMake** without installing the heavy Visual Studio IDE (saving 20+ GB of disk space).

### Why a C++ Compiler is Required
CMake is a build configuration generator, not a compiler itself. When you run `cmake -B build`, CMake searches your system for an installed C++ compiler to compile the C++ source files into `.vst3` and `.exe` binaries. 

On Windows, standard MinGW GCC is explicitly not supported by JUCE 8. Therefore, you need either **LLVM Clang** or the lightweight **MSVC Command-Line Build Tools**.

---

### Step 1: Install a Lightweight Compiler (Choose One)

#### Option A: LLVM Clang & Ninja (Recommended, ~150 MB)
If you do not want any Microsoft Visual Studio components on your system, install LLVM Clang and the Ninja build system using Windows Package Manager (`winget`):

Open **PowerShell** and run:
```powershell
winget install LLVM.LLVM
winget install Ninja-build.Ninja
```

#### Option B: MSVC Build Tools (Command-Line Only, No IDE)
If you prefer the official Microsoft C++ compiler without the Visual Studio GUI editor:

Open **PowerShell** and run:
```powershell
winget install Microsoft.VisualStudio.2022.BuildTools --override "--passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

> [!NOTE]
> After running either `winget` command, close and reopen your PowerShell terminal or VS Code so the new compiler paths are loaded into your environment `PATH`.

---

## 2. Building the Project

### Method A: Using VS Code (Recommended)

1. Open the project root in VS Code:
   ```powershell
   code "c:\Users\asus\Documents\Juce Projects\Floweret"
   ```

2. **Select Compiler Kit**:
   - When the **CMake Tools** extension prompts you at the bottom status bar, choose your installed compiler (e.g. `Clang` or `Visual Studio Build Tools - amd64`).

3. **Build**:
   - Press **`Ctrl + Shift + B`** (or click **Build** in the bottom status bar).

4. **Run & Test**:
   - Press **`F5`** to launch and debug the standalone synthesizer application immediately.

---

### Method B: Using PowerShell / Terminal

If you prefer building directly from the command line:

#### 1. With Clang & Ninja:
```powershell
# Navigate to the project root
cd "c:\Users\asus\Documents\Juce Projects\Floweret"

# Configure build with Ninja generator
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# Compile the VST3 and Standalone executable
cmake --build build --config Release
```

#### 2. With MSVC Build Tools:
```powershell
cd "c:\Users\asus\Documents\Juce Projects\Floweret"

# Configure
cmake -B build

# Compile
cmake --build build --config Release
```

---

## 3. Build Artifacts & Testing

Once compilation completes, your binaries are placed in:

| Target | File Path | Usage |
| :--- | :--- | :--- |
| **VST3 Plugin** | `build/Floweret_artefacts/Release/VST3/Floweret.vst3` | Copy to `C:\Program Files\Common Files\VST3` to load into FL Studio, Ableton Live, Bitwig, or Reaper. |
| **Standalone App** | `build/Floweret_artefacts/Release/Standalone/Floweret.exe` | Double-click to launch Floweret directly as an independent desktop audio instrument. |

---

## 4. Troubleshooting Common Build Issues

### Error: `Running 'nmake' '-?' failed with: no such file or directory`
- **Cause**: CMake defaulted to `NMake Makefiles` because no compiler environment was active in your terminal.
- **Fix**: 
  1. Delete the failed cache folder:
     ```powershell
     Remove-Item -Recurse -Force build
     ```
  2. Install Clang & Ninja via `winget install LLVM.LLVM` and `winget install Ninja-build.Ninja`.
  3. Re-run: `cmake -B build -G "Ninja"`.

### Error: `Ignoring extra path from command line: Release`
- **Cause**: Two commands were pasted on a single line without separation (e.g. `cmake -B buildcmake --build ...`).
- **Fix**: Run the configuration step (`cmake -B build -G "Ninja"`) and the build step (`cmake --build build`) as two separate commands, or separate them with a semicolon:
  ```powershell
  cmake -B build -G "Ninja"; cmake --build build --config Release
  ```

---

## 5. Codebase Directory Structure

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

## 6. How to Extend Floweret

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
