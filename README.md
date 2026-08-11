# Angry Chihuahua V1

A multiband saturation audio effects plugin built with [JUCE](https://juce.com), by [Electronic Youth](https://www.electronicyouth.co.uk).

Available as:
- **AU** (Audio Unit) — macOS, for Logic Pro / GarageBand
- **VST3** — macOS and Windows, for Ableton Live, Cubase, Studio One, Reaper, Bitwig, FL Studio, and most other DAWs

## Controls

| Parameter | Range | Description |
|---|---|---|
| **Saturation** | 0.0 – 1.0 | Amount of multiband saturation applied |
| **Cutoff** | 20 Hz – 20,000 Hz | Band split frequency |
| **Mix** | 0.0 – 1.0 | Dry/wet blend |

## For Users: Installing the Plugin

If you just want to use the plugin and don't need to build it yourself, grab the latest built VST3/AU/installer from the [Releases](../../releases) page (or the artifacts on the [Actions](../../actions) tab if no release is published yet), and drop it into your system's plugin folder:

- **macOS AU:** `~/Library/Audio/Plug-Ins/Components/`
- **macOS/Windows VST3:** `~/Library/Audio/Plug-Ins/VST3/` (macOS) or `C:\Program Files\Common Files\VST3\` (Windows)

Then rescan plugins in your DAW.

## For Developers: Building from Source

### Requirements

- [JUCE 8.0.13](https://juce.com/get-juce)
- **macOS:** Xcode, with JUCE installed at `/Applications/JUCE`
- **Windows:** Visual Studio 2022, with JUCE installed at `C:\JUCE`, plus [Inno Setup 6](https://jrsoftware.org/isinfo.php) if you want to build the installer

> The Xcode/Visual Studio project files reference JUCE at those exact paths. If your JUCE install lives somewhere else, point the project at it, e.g. on macOS:
> ```bash
> sed -i '' "s|/Applications/JUCE|/path/to/your/JUCE|g" "Builds/MacOSX/Angry Chihuahua V1.xcodeproj/project.pbxproj"
> ```

### Clone

```bash
git clone https://github.com/Eyouth1/Angry-Chihuahua-V1.git
cd Angry-Chihuahua-V1
```

### Build on macOS

```bash
xcodebuild -project "Builds/MacOSX/Angry Chihuahua V1.xcodeproj" -scheme "Angry Chihuahua V1 - AU" -configuration Release
xcodebuild -project "Builds/MacOSX/Angry Chihuahua V1.xcodeproj" -scheme "Angry Chihuahua V1 - VST3" -configuration Release
```

Built plugins land in `Builds/MacOSX/build/Release/`. Validate the AU before loading it in a DAW:

```bash
auval -v aufx Lhks Manu
```

Then copy it into place and (if needed) force your DAW to rescan:

```bash
cp -R "Builds/MacOSX/build/Release/Angry Chihauhua V1.component" ~/Library/Audio/Plug-Ins/Components/
killall -9 AudioComponentRegistrar   # if your DAW doesn't pick it up automatically
```

### Build on Windows

Open `Builds/VisualStudio2022/Angry Chihuahua V1.sln` in Visual Studio 2022 and build the `VST3` target in Release/x64, or from the command line:

```cmd
msbuild "Builds\VisualStudio2022\Angry Chihuahua V1.sln" /p:Configuration=Release /p:Platform=x64
```

Built VST3 lands in `Builds\VisualStudio2022\x64\Release\VST3\`.

### CI Builds

`.github/workflows/build.yml` builds VST3 + AU (macOS) and VST3 + installer (Windows) on GitHub-hosted runners. It's currently manual-trigger only (`workflow_dispatch`) — run it from the **Actions** tab.

## More Documentation

- [`SETUP_AND_BUILD_GUIDE.md`](SETUP_AND_BUILD_GUIDE.md) — full setup walkthrough, including Windows VM builds
- [`KNOWN_ISSUES_AND_SOLUTIONS.md`](KNOWN_ISSUES_AND_SOLUTIONS.md) — history of build issues and fixes
- [`CREDITS.md`](CREDITS.md) — attribution
- [`LICENSE`](LICENSE) — MIT

## Credits

The saturation algorithm is adapted from [MultiBandSaturation](https://github.com/raphaelmayer/MultiBandSaturation) by Raphael Mayer. See [`CREDITS.md`](CREDITS.md) for full attribution.
