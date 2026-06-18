# Angry Chihuahua V1 - Complete Setup & Build Guide

**Last Updated:** June 18, 2026  
**Project:** Angry Chihuahua V1 (Audio Plugin - VST3/AU)  
**Company:** Electronic Youth

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Development Setup](#development-setup)
3. [macOS Build (AU)](#macos-build-au)
4. [Windows Build (VST3) - Proxmox VM](#windows-build-vst3---proxmox-vm)
5. [GitHub Actions Workflow](#github-actions-workflow)
6. [Troubleshooting](#troubleshooting)
7. [Workflow Summary](#workflow-summary)

---

## Project Overview

**Angry Chihuahua V1** is a JUCE-based audio effects plugin:
- **macOS:** AU (Audio Unit) format, built in Xcode
- **Windows:** VST3 format, built in Visual Studio 2022
- **Framework:** JUCE 8.0.13
- **Language:** C++ (C++17)
- **GitHub:** https://github.com/Eyouth1/Angry-Chihuahua-V1

### Key Requirements
- **macOS:** Xcode, JUCE 8.0.13 in `~/Applications/JUCE`
- **Windows:** Visual Studio 2022, JUCE 8.0.13 in `C:\JUCE`, Inno Setup 6

---

## Development Setup

### On macOS (Development Machine)

#### 1. Install JUCE 8.0.13

```bash
mkdir -p ~/Applications
cd ~/Applications
curl -L https://github.com/juce-framework/JUCE/releases/download/8.0.13/JUCE-8.0.13-osx.zip -o JUCE.zip
unzip -q JUCE.zip
mv JUCE-8.0.13 JUCE
rm JUCE.zip
```

**Verify:**
```bash
ls ~/Applications/JUCE/Projucer.app
```

#### 2. Clone Repository

```bash
cd ~/path/to/projects
git clone https://github.com/Eyouth1/Angry-Chihuahua-V1.git
cd Angry-Chihuahua-V1/Angry_Chihuahua\ V1
```

#### 3. Project Structure

```
Angry_Chihuahua V1/
├── Source/                          # Audio code (EDIT THESE)
│   ├── PluginProcessor.h/.cpp       # DSP processing logic
│   └── PluginEditor.h/.cpp          # GUI implementation
├── Resources/                       # Images & assets
│   ├── aggressive_image.png
│   ├── calm_image.png
│   └── knob_medium.png
├── JuceLibraryCode/                 # Auto-generated (DO NOT EDIT)
├── Builds/
│   ├── MacOSX/                      # Xcode projects
│   └── VisualStudio2022/            # Visual Studio projects
├── packaging/
│   └── installer.iss                # Windows installer script
├── prompts/
│   └── VS_STUDIO_AI_PROMPT.md       # AI assistant guidelines for Windows VM
├── Angry_Chihuahua_V1.jucer         # JUCE project file (DO NOT EDIT)
└── .github/workflows/
    └── build.yml                    # GitHub Actions CI/CD
```

---

## macOS Build (AU)

### Build in Xcode

```bash
cd Angry_Chihuahua\ V1
open "Builds/MacOSX/Angry Chihuahua V1.xcodeproj"
```

**In Xcode:**
1. Select scheme: **"Angry Chihuahua V1 - AU"**
2. Set configuration: **Release**
3. Click **Product** → **Build**

**Output location:**
```
Builds/MacOSX/build/Release/AU/Angry Chihuahua V1.component
```

### Install & Test Locally

```bash
cp -r "Builds/MacOSX/build/Release/AU/Angry Chihuahua V1.component" \
  ~/Library/Audio/Plug-Ins/Components/
```

Restart your DAW and rescan plugins.

---

## Windows Build (VST3) - Proxmox VM

### Part 1: Create Proxmox VM

**Specs:**
- **OS:** Windows Server 2022 Evaluation (180 days free)
- **CPU:** 4-6 cores
- **RAM:** 8-16 GB
- **Disk:** 80 GB

See `PROXMOX_WINDOWS_BUILD_VM_SETUP.md` for detailed VM creation steps.

### Part 2: Install Build Tools on Windows VM

1. **Windows Updates** (takes ~30 min)
2. **Git:** https://git-scm.com/download/win
3. **Visual Studio 2022 Build Tools OR Community Edition**
   - Download: https://visualstudio.microsoft.com/visual-cpp-build-tools/
   - Select: "Desktop development with C++"
4. **JUCE 8.0.13:**
   ```cmd
   mkdir C:\JUCE
   cd C:\
   # Download and extract JUCE-8.0.13-windows.zip to C:\JUCE
   ```
5. **Inno Setup 6:** https://jrsoftware.org/isdl.php

### Part 3: Build on Windows VM

**Clone repository:**
```cmd
cd C:\
git clone https://github.com/Eyouth1/Angry-Chihuahua-V1.git
cd "Angry-Chihuahua-V1\Angry_Chihuahua V1"
```

**Build VST3 plugin:**
```cmd
cd "Builds\VisualStudio2022"
msbuild "Angry Chihuahua V1.sln" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142
```

**If v142 fails, try v143:**
```cmd
msbuild "Angry Chihuahua V1.sln" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143
```

**Output location:**
```
Builds\VisualStudio2022\x64\Release\VST3\Angry Chihuahua V1.vst3
```

### Part 4: Create Windows Installer (Optional)

```cmd
cd "C:\Angry-Chihuahua-V1\Angry_Chihuahua V1"
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /O"Builds\VisualStudio2022\x64\Release" packaging\installer.iss
```

**Installer output:**
```
Builds\VisualStudio2022\x64\Release\Angry-Chihuahua-V1-Windows-VST3-Installer.exe
```

### Part 5: Install & Test Plugin

```cmd
copy "Builds\VisualStudio2022\x64\Release\VST3\Angry Chihuahua V1.vst3" "C:\Program Files\Common Files\VST3\"
```

Or run the installer EXE.

Open a DAW, rescan plugins, load "Angry Chihuahua V1".

---

## GitHub Actions Workflow

### Automated Build on Push

**Trigger:** When you push to GitHub  
**Jobs:** 
- Build macOS VST3 + AU
- Build Windows VST3 + installer EXE
- Upload artifacts

**Artifacts available after successful build:**
- `Angry-Chihuahua-V1-macOS` — macOS VST3
- `Angry-Chihuahua-V1-Windows` — Windows VST3 files
- `Angry-Chihuahua-V1-Installer` — Windows installer EXE

**View builds:** https://github.com/Eyouth1/Angry-Chihuahua-V1/actions

---

## Development Workflow

### Daily Development

1. **On Mac:**
   ```bash
   # Edit source code
   nano "Source/PluginProcessor.cpp"
   
   # Test locally in Xcode
   open "Builds/MacOSX/Angry Chihuahua V1.xcodeproj"
   ```

2. **Commit & Push:**
   ```bash
   git add Source/
   git commit -m "Improve audio processing"
   git push origin main
   ```

3. **GitHub Actions runs automatically**
   - Builds macOS AU
   - Builds Windows VST3
   - Creates installer EXE
   - Uploads artifacts

4. **Download & Test:**
   - Go to GitHub Actions
   - Download artifacts
   - Test in DAW on respective platform

### If You Modify Project Structure

**If you add/remove source files:**

1. **On Mac:**
   ```bash
   # Open Projucer
   open ~/Applications/JUCE/Projucer.app
   # Open: Angry_Chihuahua_V1.jucer
   # Add/remove files
   # Save (regenerates VS & Xcode projects)
   ```

2. **Commit regenerated files:**
   ```bash
   git add Builds/
   git commit -m "Update project structure"
   git push origin main
   ```

3. **On Windows VM:** Pull fresh and rebuild

---

## Troubleshooting

### Windows VM Build Issues

| Issue | Solution |
|-------|----------|
| **"juce.h not found"** | Verify `C:\JUCE\modules` exists with JUCE files |
| **Unresolved external symbol** | Ensure Release/x64 configuration selected |
| **Platform toolset mismatch** | Try `/p:PlatformToolset=v142` or `v143` in msbuild |
| **JuceLibraryCode not found** | Run Git LFS or re-clone: `git clone https://github.com/Eyouth1/Angry-Chihuahua-V1.git` |
| **VST3 not in DAW** | Copy to `C:\Program Files\Common Files\VST3\` and rescan |
| **Installer fails** | Verify Inno Setup 6 installed; check `packaging\installer.iss` |

### macOS Build Issues

| Issue | Solution |
|-------|----------|
| **"JUCE not found"** | Install JUCE: `~/Applications/JUCE` with modules folder |
| **Xcode scheme not found** | Run Projucer: open `.jucer` file and save |
| **Build hangs** | Close other Xcode windows; increase Xcode build memory |

### GitHub Actions Issues

| Issue | Solution |
|-------|----------|
| **Build fails on macOS runner** | JUCE download issue — check workflow logs; may retry |
| **Build fails on Windows runner** | Check console output; usually toolset or JUCE path issue |
| **Artifacts not uploaded** | Build may have failed; check logs on GitHub Actions page |

---

## Important Files (DO NOT EDIT)

❌ `.jucer` file — Auto-generated configuration  
❌ `JuceLibraryCode/` — Auto-generated by Projucer  
❌ `Builds/*.vcxproj` — Auto-generated project files  
❌ `.sln` files — Auto-generated solutions  

**Regenerate from Mac if structure changes:**
1. Modify `.jucer` in Projucer
2. Save (regenerates files)
3. Commit to GitHub
4. Re-clone on Windows VM

---

## VS Studio AI Assistant

**On Windows VM:** Use the AI prompt in `prompts/VS_STUDIO_AI_PROMPT.md`

**What the AI can help with:**
- ✅ C++/JUCE code questions
- ✅ Debug compiler errors
- ✅ Audio processing improvements
- ✅ UI/GUI modifications

**What the AI CANNOT do:**
- ❌ Push to GitHub from VM (changes go Mac-first)
- ❌ Modify `.jucer` or auto-generated files
- ❌ Change project structure without Projucer

---

## Resource Links

- **JUCE Documentation:** https://docs.juce.com/
- **JUCE Forums:** https://forum.juce.com/
- **VST3 Spec:** https://steinbergmedia.github.io/vst3_dev_portal/
- **GitHub Repo:** https://github.com/Eyouth1/Angry-Chihuahua-V1
- **GitHub Actions:** https://github.com/Eyouth1/Angry-Chihuahua-V1/actions

---

## Summary

| Platform | Build | Tools | Output |
|----------|-------|-------|--------|
| **macOS** | Xcode | JUCE + Xcode | AU plugin |
| **Windows** | Visual Studio | JUCE + VS Build Tools | VST3 plugin + installer |
| **CI/CD** | GitHub Actions | Runs on cloud | All artifacts |

**Workflow:** Edit on Mac → Push → GitHub Actions builds both → Download artifacts

---

## Quick Reference

```bash
# Mac build
cd "Builds/MacOSX"
open "Angry Chihuahua V1.xcodeproj"
# Select AU scheme, click Build

# Windows build (on VM)
cd "Builds\VisualStudio2022"
msbuild "Angry Chihuahua V1.sln" /p:Configuration=Release /p:Platform=x64

# Create Windows installer
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /O"Builds\VisualStudio2022\x64\Release" packaging\installer.iss
```

---

**Good luck! 🎵 For questions, see GitHub repo or check the prompts/ folder for AI guidance.**
