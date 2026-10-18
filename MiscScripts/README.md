# Background Removal & Logo Tools

A toolchain for removing solid backgrounds from logos, typography, and UI graphics with **subpixel anti-aliasing preservation** and **automatic halo elimination**.

---

## Features

- **Halo-Free De-matting**: Completely un-mattes the edge pixels so that when the asset is placed onto Floweret's soft oatmeal (`#ECE4DB`) background, there are zero white or gray halos.
- **Multiple Run Modes**:
  1. **Drag-and-Drop** (`remove_background.bat`)
  2. **Interactive CLI Wizard** (no arguments needed)
  3. **Direct CLI / Automation** (PowerShell or Python)
- **Direct Plugin Embedding**: Can automatically optimize and embed the logo into `Source/UI/FloweretLogoData.h` with the `--embed` flag.

---

## Usage

### 1. Drag & Drop (Windows Explorer)

Drag any image (`.png`, `.jpg`, `.jpeg`) from Windows Explorer and drop it directly onto:

```
remove_background.bat
```

It will generate `<image_name> (transparent).png` and prompt if you'd like to update the plugin header.

### 2. Interactive Menu

Simply double-click `remove_background.bat` or run:

```powershell
python "MiscScripts/remove_background.py"
```

It will display an interactive list of assets in `Assets/Logo/` and guide you through the process.

### 3. Command Line (Python or PowerShell)

#### Using Python:

```bash
# Convert an image
python "MiscScripts/remove_background.py" "Assets/Logo/logo 3.png"

# Convert, crop for UI, and immediately embed into Floweret's C++ header
python "MiscScripts/remove_background.py" "Assets/Logo/logo 3.png" --crop --embed
```

#### Using PowerShell:

```powershell
# Convert and embed
.\MiscScripts\remove_background.ps1 "Assets\Logo\logo 3.png" -Crop -Embed
```

---

## Options

| Flag             | Description                                                                                                  |
| ---------------- | ------------------------------------------------------------------------------------------------------------ |
| `-o`, `--output` | Custom destination path for the transparent PNG.                                                             |
| `--crop`         | Saves an optically balanced, tightly padded version (`floweret_logo_transparent.png`).                       |
| `--embed`        | Encodes and writes the PNG directly into `Source/UI/FloweretLogoData.h`.                                     |
| `--bg`           | Specify background color (`white`, `black`, or hex like `#FFFFFF`). Defaults to auto-detecting border color. |
| `--threshold`    | Sensitivity threshold (default: `3.0`).                                                                      |
