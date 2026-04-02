# OBS Timeline Monitor

An OBS Studio plugin that displays a real-time progress bar and time indicator for every media source (video/audio files) in your current scene.

## Features

- Live progress bar for each media source in the active scene
- Elapsed and remaining time display
- Auto-detects media sources — no configuration needed
- Works with nested scenes and groups
- Supports `ffmpeg_source` (Media Source) and `vlc_source` (VLC Source)

## Requirements

- OBS Studio 32.x or later

## Installation

### macOS

1. Download `obs-timeline-monitor-macos.zip` from the [Releases](../../releases) page
2. Unzip and copy `obs-timeline-monitor.plugin` to the plugin folder.
3. Restart OBS
4. Go to **Docks → obs-timeline-monitor**

> **Note:** macOS may warn about an unidentified developer on first launch.  
> Go to System Settings → Privacy & Security and click **Open Anyway**.

### Windows

1. Download `obs-timeline-monitor-windows.zip` from the [Releases](../../releases) page
2. Unzip and copy the contents to your OBS installation folder:
   - `obs-plugins/64bit/obs-timeline-monitor.dll` → `C:\Program Files\obs-studio\obs-plugins\64bit\`
   - `data/obs-plugins/obs-timeline-monitor/` → `C:\Program Files\obs-studio\data\obs-plugins\`
3. Restart OBS

### Linux

1. Download `obs-timeline-monitor-linux.tar.gz` from the [Releases](../../releases) page
2. Extract and copy:
   - `obs-timeline-monitor.so` → `~/.config/obs-studio/plugins/obs-timeline-monitor/bin/64bit/`
   - `locale/` → `~/.config/obs-studio/plugins/obs-timeline-monitor/data/locale/`
3. Restart OBS

---

## Building from source

### macOS

**Requirements:** Xcode, CMake, Homebrew

```bash
# Install dependencies
brew install cmake qt@6 simde

# Clone and build
git clone https://github.com/yourname/obs-timeline-monitor.git
cd obs-timeline-monitor
./deploy.sh
```

The script clones OBS Studio sources automatically, compiles the plugin, and installs it into OBS.app.

### Windows

**Requirements:** Visual Studio 2022, CMake, OBS Studio installed

```bat
cmake -S . -B build ^
  -DCMAKE_PREFIX_PATH="C:\Program Files\obs-studio\cmake" ^
  -DQt6_DIR="<path-to-qt>\lib\cmake\Qt6"
cmake --build build --config Release
```

### Linux

**Requirements:** CMake, Qt6, OBS Studio dev packages

```bash
# Ubuntu/Debian
sudo apt install cmake qt6-base-dev libobs-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build
```

---

## License

MIT
