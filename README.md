# Display VCP

A KDE system tray application
which provides a Virtual Control Panel (VCP) to control display features
like brightness, contrast, etc. and other manufacturer-specific features.

> [!WARNING]
> Use this work at your own risk.
>
> This project is neither robust nor compatible with display models other than
> what's hardcoded.

## Build

```sh
# Build tools
sudo apt install build-essential cmake

# Build dependencies
sudo apt install libkf6statusnotifieritem-dev libkf6coreaddons-dev
# This should also install `qt6-base-dev` automatically
# OpenSUSE: kf6-kcoreaddons-devel kf6-kstatusnotifieritem-devel qt6-base-devel

# Generate build files
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build ./build --config Release --target all
```

## Run

Requirements:

- KDE Desktop
- ddcutil
- A single display monitor with DDC/CI (Display Data Channel / Command Interface)
  support

```sh
# Install ddcutil
sudo zypper install ddcutil

# Run the executable
./build/display-vcp-tray
```

## Similar Projects

- MacOS
  - https://github.com/alin23/Lunar
  - https://github.com/MonitorControl/MonitorControl
- Windows
  - https://github.com/emoacht/Monitorian
- [bebump/broitness](https://github.com/bebump/broitness)
- [angelodlfrtr/ddc-control-tray](https://github.com/angelodlfrtr/ddc-control-tray)
- [Jakeler/ddc-tray](https://github.com/Jakeler/ddc-tray)
