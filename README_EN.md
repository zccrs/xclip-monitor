# X11 Clipboard Monitor

**Language / 语言:** [English](README_EN.md) | [中文](README.md)

This is an X11 clipboard monitoring tool written in C that can monitor clipboard changes in real-time and display detailed information about the clipboard owner.

## 🌐 Multi-language Support

This tool now supports bilingual display in Chinese and English:
- 🔍 **Automatic Language Detection**: Automatically selects display language based on system environment variables (`LANG`, `LC_ALL`, `LC_MESSAGES`)
- 🇨🇳 **Chinese Support**: Chinese locales like `zh_CN`, `zh_TW`, `zh_HK`, `zh_SG`, etc.
- 🇺🇸 **English Support**: Other locales default to English display
- 🛠️ **Manual Setting**: Language can be manually specified by setting environment variables

### Usage Examples
```bash
# Display in Chinese
LANG=zh_CN.UTF-8 ./xclip-monitor

# Display in English
LANG=en_US.UTF-8 ./xclip-monitor
```

## Features

- 🔍 Real-time monitoring of X11 clipboard changes
- 📋 Get clipboard owner window information
- 🏷️ Display application name and class
- 🆔 Use XRes extension to get process ID (required)
- 📝 Display complete process command line information
- ⏰ Real-time display of change timestamps
- 🎯 Low CPU usage polling mechanism

## System Requirements

- Linux system (with X11 support)
- X11 development libraries
- XRes extension support
- GCC compiler

## Dependency Installation

### Ubuntu/Debian Systems
```bash
sudo apt update
sudo apt install build-essential libx11-dev libxres-dev
```

### CentOS/RHEL/Fedora Systems
```bash
# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install libX11-devel libXres-devel

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install libX11-devel libXres-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel libx11 libxres
```

## Compilation and Installation

### Building with CMake

#### Quick Compilation
```bash
mkdir build
cd build
cmake ..
make
```

#### Specify Build Type
```bash
# Debug build (includes debug information)
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build (optimized version)
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

#### Install to System Path
```bash
sudo make install
```

#### Uninstall
```bash
make uninstall
```

#### Clean Build Files
```bash
# Clean build directory
rm -rf build
```

## Usage

### Basic Run
```bash
./xclip-monitor
```

### Run in Background and Log
```bash
./xclip-monitor > clipboard.log 2>&1 &
```

### Runtime Output Example
```
=== X11 Clipboard Monitor ===
Function: Monitor clipboard changes and show owner process information

X11 clipboard monitor started
Current clipboard owner: 0x2200003

===============================================
Clipboard Owner Information:
Window ID: 0x2200003
Window Name: Firefox
Application Class: firefox
Application Name: Navigator
Process ID: 12345
Command Line: /usr/lib/firefox/firefox --new-instance
===============================================

Monitoring clipboard changes... (Press Ctrl+C to exit)

Clipboard change detected! Time: 2024-01-15 14:30:25
===============================================
Clipboard Owner Information:
Window ID: 0x3400004
Window Name: Visual Studio Code
Application Class: code
Application Name: code
Process ID: 23456
Command Line: /usr/share/code/code --unity-launch
===============================================
```

## Technical Implementation

### Core Technologies
- **X11 Selection API**: Monitor clipboard ownership changes
- **XRes Extension**: Get process ID of window owner (required)
- **/proc File System**: Read process command line information

### Working Principle
1. Initialize X11 connection and clipboard atoms
2. Periodically poll clipboard owner status
3. Get new owner window information when changes are detected
4. Use XRes extension to get process ID of window owner
5. Read command line from /proc/[pid]/cmdline

### Process ID Acquisition Strategy
- **Only Method**: Use XRes extension's XResQueryClientIds API
- **Error Handling**: Display appropriate messages when unable to obtain

## File Structure

```
xclip-monitor/
├── xclip_monitor.c           # Main source code file
├── CMakeLists.txt           # CMake build configuration
├── cmake_uninstall.cmake.in # CMake uninstall script template
├── test_demo.sh            # Demo script
├── README.md               # Project documentation (Chinese)
├── README_EN.md            # Project documentation (English)
└── build/                  # CMake build directory
    └── xclip-monitor       # Compiled executable
```

## Troubleshooting

### Common Issues

#### 1. Compilation Error: X11 Header Files Not Found
```
fatal error: X11/Xlib.h: No such file or directory
```
**Solution**: Install X11 development packages
```bash
sudo apt install libx11-dev libxres-dev  # Ubuntu/Debian
```

#### 2. XRes Extension Not Available
Program displays "XRes extension not available" when running
**Solution**: Ensure X server supports XRes extension, which is required for the program to run

#### 3. Cannot Get Process ID
Program displays "Cannot get process ID via XRes"
**Solution**: This may be due to XRes extension version incompatibility or permission restrictions, the tool will display "Cannot retrieve"

#### 4. Permission Issues
Unable to read /proc/[pid]/cmdline for certain processes
**Solution**: This is a system security mechanism and is normal behavior

### Debugging Tips

#### Check X11 Environment
```bash
echo $DISPLAY
xdpyinfo | grep -i clipboard
```

#### View Clipboard Status
```bash
xclip -selection clipboard -o  # View current clipboard content
xprop -root | grep CLIPBOARD   # View clipboard properties
```

## Development Notes

### Code Structure
- `ClipboardMonitor`: Main data structure
- `init_clipboard_monitor()`: Initialization function
- `monitor_clipboard_changes()`: Main monitoring loop
- `get_window_pid()`: Get process ID using XRes extension
- `print_window_info()`: Information display

### Suggested Feature Extensions
- Add clipboard content recording functionality
- Support PRIMARY selection monitoring
- Add filtering and rule configuration
- Support network remote monitoring
- Integrate system notifications

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contributing

Issues and Pull Requests are welcome to improve this tool.

## Author

Created by AI assistant for X11 clipboard monitoring needs.
