# Known Issues & Solutions

**Document summarizing problems encountered and their solutions.**

---

## Issue #1: GitHub Actions Build Failures (Week-long issue)

### Problem

GitHub Actions workflow kept failing for **both macOS and Windows** builds with various errors:

**macOS error:**
```
mv: rename JUCE-8.0.13 to JUCE/JUCE-8.0.13: No such file or directory
```

**Windows error:**
```
error C1083: Cannot open source file: 'JuceLibraryCode/include_juce_audio_basics.cpp'
```

### Root Causes

1. **macOS:** JUCE directory already existed from previous run, `mv` tried to move into it instead of replacing
2. **Windows:** 
   - Path handling issues with spaces in filenames
   - `vcvars64.bat` invocation syntax problems
   - Mismatch between local paths and GitHub Actions runner paths

### Solutions Applied

✅ **macOS fix:**
```bash
# Remove old JUCE before downloading new one
rm -rf /Applications/JUCE
```

✅ **Windows fixes:**
```cmd
# Use cd /d to handle drive changes
cd /d "D:\a\Angry-Chihuahua-V1"

# Use 'call' instead of && for vcvars64.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"

# Add explicit platform toolset
msbuild /p:PlatformToolset=v143
```

### Current Status

✅ **Partially Fixed** — Workflow should now build successfully, but alternative Windows build approach recommended (Proxmox VM)

---

## Issue #2: Missing JUCE Installation on macOS

### Problem

User deleted JUCE from `~/Applications/` but needed to keep it for macOS AU builds (different from Windows VST3 builds).

### Root Cause

User was consolidating installations and deleted old JUCE directories, accidentally removing the active one.

### Solution

✅ **Reinstall JUCE:**
```bash
mkdir -p ~/Applications
cd ~/Applications
curl -L https://github.com/juce-framework/JUCE/releases/download/8.0.13/JUCE-8.0.13-osx.zip -o JUCE.zip
unzip -q JUCE.zip
mv JUCE-8.0.13 JUCE
rm JUCE.zip
```

### Current Status

✅ **Resolved** — JUCE now properly installed in `~/Applications/JUCE`

---

## Issue #3: Multiple Duplicate JUCE Installations

### Problem

Found 7 JUCE directories on macOS:
- `~/Applications/JUCE` (current)
- `~/Applications/JUCE_old` through `JUCE_old(6)` (old backups)

**Total disk waste: ~1GB**

### Root Cause

Mac's version control on the Applications folder kept creating backups during updates.

### Solution

✅ **Cleaned up old versions:**
```bash
rm -rf ~/Applications/JUCE_old*
```

### Current Status

✅ **Resolved** — Only one active JUCE installation remains

---

## Issue #4: Project Structure Confusion (File Names)

### Problem

Project had files with spaces in names:
- `Angry Chihuahua V1.jucer`
- `Angry Chihuahua V1.code-workspace`

These caused issues with:
- Git workflows (path escaping)
- Build systems (space handling)

### Root Cause

JUCE Projucer generates files with project name, which had spaces.

### Solution

✅ **Renamed project files** (when possible):
```
Angry Chihuahua V1.jucer → Angry_Chihuahua V1.jucer
```

### Current Status

✅ **Partially Resolved** — Main `.jucer` still has spaces (can't rename - Projucer requirement), but workflow accounts for it

---

## Issue #5: Windows VM Missing JuceLibraryCode on Clone

### Problem

When cloning repo on Windows VM, `JuceLibraryCode/` directory was empty or missing.

### Root Cause

Possible Git LFS issue or incomplete clone transfer.

### Solution

**Option A (Recommended):**
```cmd
# Install Git LFS on Windows VM
choco install git-lfs -y
git lfs install

# Re-clone repo
git clone https://github.com/Eyouth1/Angry-Chihuahua-V1.git
```

**Option B:**
```cmd
# Manual: Copy JuceLibraryCode from GitHub or another machine
```

### Current Status

⚠️ **Needs Testing** — Apply solution when cloning on Windows VM

---

## Issue #6: Platform Toolset Mismatch (Windows Build)

### Problem

Windows VM build fails with toolset version mismatch between project and Visual Studio 2022 installation.

### Root Cause

Project was generated with `v142` toolset (VS 2019), but Windows VM has `v143` (VS 2022).

### Solution

✅ **Override toolset in build command:**
```cmd
# Try v142 first (original)
msbuild /p:PlatformToolset=v142

# If that fails, use v143 (VS 2022)
msbuild /p:PlatformToolset=v143
```

### Current Status

✅ **Resolved** — Build guide includes both options

---

## Issue #7: AI Assistant on Windows VM Making Unwanted Changes

### Problem

An AI tool running on the Windows VM modified source code without proper safeguards.

### Root Cause

AI assistant didn't have clear guidelines about what could/couldn't be changed.

### Solution

✅ **Created system prompt:**
- `prompts/VS_STUDIO_AI_PROMPT.md`
- Defines boundaries and best practices
- Lists files that should NEVER be edited
- Explains workflow for code changes

### Current Status

✅ **Resolved** — Prompt file in repo with clear guidelines

---

## Issue #8: Choosing Between GitHub Actions and Local Windows Build

### Problem

GitHub Actions had persistent issues; user needed a working Windows build environment.

### Root Cause

Complex environment setup on GitHub Actions runners; easier to have local control.

### Solution

✅ **Set up Proxmox Windows VM:**
- Full control over build environment
- Reusable for future projects
- Faster iteration during development
- GitHub Actions still works for CI/CD

**Workflow:**
- **Development:** Build locally on Windows VM
- **Distribution:** GitHub Actions creates release artifacts

### Current Status

✅ **Implemented** — Proxmox VM setup guide provided in `PROXMOX_WINDOWS_BUILD_VM_SETUP.md`

---

## Issue #9: Project Directory Organization

### Problem

Build outputs were ending up in non-standard locations:
- `Angry_Chihuahua_Windows_Release/` (custom directory)
- Instead of: `Builds/VisualStudio2022/x64/Release/`

### Root Cause

Manual setup or Inno Setup outputting to unexpected locations.

### Solution

✅ **Standardized build paths:**
```cmd
# Standard output
Builds/VisualStudio2022/x64/Release/VST3/
Builds/VisualStudio2022/x64/Release/Angry-Chihuahua-V1-Windows-VST3-Installer.exe
```

✅ **Added to .gitignore:**
```
Builds/VisualStudio2022/x64/
Builds/VisualStudio2022/Win32/
Builds/MacOSX/build/
```

### Current Status

✅ **Resolved** — Standard paths documented and enforced

---

## Lessons Learned

### For This Project

1. **JUCE projects need careful path management** — Spaces, symbolic links, and relative paths cause issues
2. **GitHub Actions + JUCE is complex** — Local builds more reliable for development
3. **AI assistants need clear guidelines** — Without boundaries, they make unintended changes
4. **Dual-platform builds are tricky** — macOS and Windows have very different build systems
5. **Documentation is critical** — Having this guide prevents re-discovering solutions

### For Future JUCE Projects

1. Consider **CMake-based builds** (like Pamplejuce) for better portability
2. Keep **Git LFS installed** on all machines for large binary files
3. Use **CI/CD for release builds**, local machines for development
4. **Version-control all generated files** (Projucer outputs) for reproducibility
5. Create **AI prompt guidelines** when using AI assistants on development VMs

---

## Testing Checklist

- [ ] macOS AU builds in Xcode
- [ ] Windows VST3 builds on Proxmox VM
- [ ] GitHub Actions builds both platforms
- [ ] Artifacts download successfully
- [ ] Plugins load in DAW (tested on at least one DAW)
- [ ] Audio processing works (input/output connected)
- [ ] Windows installer creates .exe successfully
- [ ] Installer can be run on clean Windows machine
- [ ] AI assistant on Windows VM follows guidelines

---

## References

- **Chat Transcript:** June 18, 2026 - AI assistant & user collaboration
- **Key Files Changed:**
  - `.github/workflows/build.yml` — Fixed GitHub Actions
  - `packaging/installer.iss` — Created Windows installer
  - `prompts/VS_STUDIO_AI_PROMPT.md` — AI guidelines
  - `SETUP_AND_BUILD_GUIDE.md` — Complete setup documentation
  - `PROXMOX_WINDOWS_BUILD_VM_SETUP.md` — Windows VM setup

---

**Note:** This document serves as a reference for troubleshooting similar issues in the future or on related projects.
