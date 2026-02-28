# QFluentKit

<div align="center">

A beautiful and modern Qt Fluent Design component library with 90+ high-quality out-of-the-box components

<p align="center">
<a href="README.md">English</a> | <a href="README_ZH.md">简体中文</a>
</p>

[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5.12%2B-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/toddming/QFluentKit)

</div>

---

## Introduction

**QFluentKit** is a Qt-based Fluent Design style component library that provides a complete collection of modern UI components. The project adopts the C++17 standard, supports both Qt5/Qt6 versions, and is compatible with Windows, Linux, and macOS platforms.

<img width=866 height=670 src="images/light.png">

<img width=866 height=670 src="images/dark.png">

### Key Features

- 🎨 **Fluent Design Style** - Modern Microsoft Fluent Design UI specifications
- 🌓 **Dark/Light Theme** - Automatic theme switching with custom theme color support
- 🧩 **90+ Advanced Components** - Covering input, display, layout, dialogs, and complete scenarios
- 🦦 **Qt5/Qt6 Dual Support** - One codebase, fully compatible with both versions
- 🖥️ **Cross-Platform** - Windows, Linux, macOS support
- 📦 **Easy Integration** - Dynamic library format with simple CMake integration
- 🎭 **Rich Animation Effects** - Multiple built-in smooth animation systems
- 🎭 **Acrylic Material Effect** - Support for Windows acrylic frosted glass effect

---

## System Support

| Platform | Support Status | Remarks |
|----------|----------------|---------|
| Windows 10/11 | ✅ Full Support | Recommended, best experience |
| Linux | ✅ Supported | Qt5/Qt6 both available |
| macOS | ✅ Supported | Qt5/Qt6 both available |

---

## Requirements

### Qt Version
- **Qt 5.12** or higher
- **Qt 6.x** fully supported

### Compiler
| Platform | Recommended Compiler |
|----------|---------------------|
| Windows | MinGW 8.0+ or MSVC 2017+ |
| Linux | GCC 7+ or Clang 5+ |
| macOS | Clang (Xcode 10+) |

### Dependencies
- **CMake** 3.15+
- **Qt Modules**: Core, Widgets, Svg, Xml

---

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/toddming/QFluentKit.git
cd QFluentKit
```

### 2. Build the Project

#### Windows (MinGW)

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

#### Windows (MSVC)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

#### Linux/macOS

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

Run the example program:

```bash
cd build/QFluentExample
./QFluentExample    # Linux/macOS
QFluentExample.exe  # Windows
```

---

## Integration into Your Project

### CMake Method

Add to your `CMakeLists.txt`:

```cmake
# Minimum CMake version
cmake_minimum_required(VERSION 3.15)

# Add QFluentKit subdirectory
add_subdirectory(QFluentKit)

# Link QFluentKit
target_link_libraries(MyApp PRIVATE QFluent)

```

### Manual Method

1. Build QFluentKit to generate `QFluentKit.dll` (Windows) or `libQFluentKit.so` (Linux/macOS)
2. Include the header directory in your project: `QFluentKit/QFluent/src/`
3. Link the generated library file

### Optional: Enable QWindowKit Integration

QWindowKit provides advanced window management features (frameless windows, frosted glass effect, etc.):

```cmake
# Enable when building the example program
set(BUILD_QWINDOWKIT ON CACHE BOOL "Build with QWindowKit support" FORCE)

# Or enable via CMake parameter
cmake -DBUILD_QWINDOWKIT=ON ..
```

Once enabled, the `USE_QWINDOWKIT` macro will be defined, allowing access to enhanced window features.

---

## Basic Usage

### Hello World

```cpp
#include <QWidget>
#include <QApplication>

#include "QFluent/LineEdit.h"
#include "QFluent/PushButton.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create a Fluent window
    QWidget window;
    window.setWindowTitle("QFluentKit Example");
    window.resize(800, 600);

    // Create a button
    auto *button = new PrimaryPushButton(&window);
    button->setText("Click Me");
    button->move(350, 280);

    // Create a line edit
    auto *lineEdit = new LineEdit(&window);
    lineEdit->setPlaceholderText("Please enter content...");
    lineEdit->move(350, 340);

    window.show();
    return app.exec();
}

```

---

## Component List

### Basic Input
- Button, PrimaryButton, HyperlinkButton
- CheckBox, RadioButton
- ComboBox, LineEdit, Slider, SpinBox
- SwitchButton, PushButton

### Display Components
- Label, CaptionLabel, StrongLabel
- ImageLabel, IconWidget
- CardWidget, Loading

### Date and Time
- DatePicker, TimePicker
- CalendarPicker, CalendarView
- CycleListWidget

### Menu and Navigation
- RoundMenu, NavigationPanel, NavigationBar
- Pivot, TabBar

### Dialogs
- MessageDialog, ColorDialog, Flyout, TeachingTip

### Container and Layout
- StackedWidget, ScrollArea, TableView
- ListView, FlowLayout, ExpandLayout

### Progress and Status
- ProgressBar, ProgressRing
- IndeterminateProgressBar, IndeterminateProgressRing
- InfoBar, ToolTip

### Setting Cards
- SettingCard, SettingCardGroup
- ExpandSettingCard, OptionsSettingCard

### Material Effects
- AcrylicWidget, AcrylicLabel, AcrylicMenu, AcrylicToolTip

---

## Project Structure

```
QFluentKit/
├── QFluent/                      # Core dynamic library
│   ├── src/
│   │   ├── FluentGlobal.h        # Global enumerations (ThemeMode, IconType, ThemeStyle)
│   │   ├── Theme.h               # Theme management system (AUTO/LIGHT/DARK)
│   │   ├── Router.h              # Routing system (works with StackedWidget)
│   │   ├── FluentIcon.h          # Icon system (248+ built-in SVG icons)
│   │   ├── StyleSheet.h          # Stylesheet management system
│   │   ├── Animation.h           # Animation system base class
│   │   ├── QFluent/              # Public component headers
│   │   │   ├── BasicInput/       # Basic input components
│   │   │   ├── Display/          # Display components
│   │   │   ├── DateTime/         # Date and time components
│   │   │   ├── Menu/             # Menu components
│   │   │   ├── Dialog/           # Dialogs
│   │   │   ├── Layout/           # Layout containers
│   │   │   ├── Progress/         # Progress components
│   │   │   ├── Setting/          # Setting cards
│   │   │   └── Material/         # Material effects
│   │   └── Private/              # PIMPL private implementation
│   └── res/                      # Resource files
│       ├── images/icons/         # Fluent Design SVG icons
│       └── style/                # QSS stylesheets (light/dark)
├── QFluentExample/                # Example application
│   ├── src/
│   │   ├── Interface/            # 15 demo interfaces
│   │   │   ├── HomeInterface.h
│   │   │   ├── BasicInputInterface.h
│   │   │   └── ...
│   │   └── Window/               # Custom window examples
│   │       ├── LoginWindow.h
│   │       ├── NavbarWindow.h
│   │       └── SplitWindow.h
│   └── libs/qwindowkit/          # Optional window management library
├── CMakeLists.txt                 # Root CMake configuration
└── README.md
```

---

## License

This project is open source under the [GPLv3](LICENSE) license.

---

## Contributing

Issues and Pull Requests are welcome!

---

## Acknowledgments

- Core window management: [QWindowKit](https://github.com/stdware/qwindowkit)
- Design inspiration from: [PyQt-Fluent-Widgets](https://github.com/zhiyiYo/PyQt-Fluent-Widgets)

---

<div align="center">

⭐ If you find this project helpful, please give it a Star!

</div>