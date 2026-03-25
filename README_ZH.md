# QFluentKit

<div align="center">

一款美观的现代 Qt Fluent Design 组件库，提供 90+ 开箱即用的高质量组件

<p align="center">
<a href="README.md">English</a> | <a href="README_ZH.md">简体中文</a>
</p>

[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5.12%2B-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/toddming/QFluentKit)

</div>

---

## 简介

**QFluentKit** 是一个基于 Qt 的 Fluent Design 风格组件库，提供完整的现代化 UI 组件集合。项目采用 C++17 标准，支持 Qt5/Qt6 双版本，兼容 Windows、Linux 和 macOS 平台。

<img width=866 height=670 src="images/light.png">

<img width=866 height=670 src="images/dark.png">

### 核心特性

- 🎨 **Fluent Design 设计风格** - 现代化的 Microsoft Fluent Design UI 规范
- 🌓 **深色/浅色主题** - 自动主题切换，支持自定义主题色
- 🧩 **90+ 高级组件** - 涵盖输入、显示、布局、对话框等完整场景
- 🦦 **Qt5/Qt6 双支持** - 一套代码，两个版本完全兼容
- 🖥️ **跨平台** - 支持 Windows、Linux、macOS
- 📦 **易于集成** - 动态库形式，CMake 集成简单
- 🎭 **丰富的动画效果** - 内置多种流畅动画系统
- 🎭 **Acrylic 材质效果** - 支持 Windows 亚克力毛玻璃效果

---

## 系统支持

| 平台 | 支持状态 | 备注 |
|------|---------|------|
| Windows 10/11 | ✅ 完全支持 | 推荐，最佳体验 |
| Linux | ✅ 支持 | Qt5/Qt6 均可 |
| macOS | ✅ 支持 | Qt5/Qt6 均可 |

---

## 环境要求

### Qt 版本
- **Qt 5.12** 或更高版本
- **Qt 6.x** 完全支持

### 编译器
| 平台 | 推荐编译器 |
|------|-----------|
| Windows | MinGW 8.0+ 或 MSVC 2017+ |
| Linux | GCC 7+ 或 Clang 5+ |
| macOS | Clang (Xcode 10+) |

### 依赖工具
- **CMake** 3.15+
- **Qt Modules**: Core, Widgets, Svg, Xml

---

## 快速开始

### 1. 克隆项目

```bash
git clone https://github.com/toddming/QFluentKit.git
cd QFluentKit
```

### 2. 构建项目

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

运行示例程序：

```bash
cd build/QFluentExample
./QFluentExample    # Linux/macOS
QFluentExample.exe  # Windows
```

---

## 集成到你的项目

### 方式一：安装到 Qt 目录（推荐）

将 QFluentKit 安装到 Qt 安装目录后，所有 Qt 项目都可以直接使用。

#### 步骤 1：构建和安装

**Windows (MSVC)**
```bash
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 -DQFLUENT_INSTALL_TO_QT=ON ..
cmake --build . --config Release
cmake --install . --config Release
```

**Windows (MinGW)**
```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" -DQFLUENT_INSTALL_TO_QT=ON ..
mingw32-make
mingw32-make install
```

**Linux/macOS**
```bash
mkdir build && cd build
cmake -DQFLUENT_INSTALL_TO_QT=ON ..
make -j$(nproc)
sudo make install
```

#### 步骤 2：在项目中使用

安装完成后，只需在 `CMakeLists.txt` 中添加：

```cmake
find_package(QFluent REQUIRED)
target_link_libraries(MyApp PRIVATE QFluent::QFluent)
```

#### 安装目录结构

安装完成后，文件组织如下（以 Qt 6.8.3 MSVC 为例）：

```
E:/Qt/6.8.3/msvc2022_64/
├── bin/
│   ├── QFluent.dll           # Release 动态库
│   ├── QFluentd.dll          # Debug 动态库（带 'd' 后缀）
│   ├── QFluent.pdb           # Debug PDB 文件（仅 MSVC）
│   └── ...
├── lib/
│   ├── QFluent.lib           # Release 导入库
│   ├── QFluentd.lib          # Debug 导入库（带 'd' 后缀）
│   └── cmake/
│       └── QFluent/          # CMake 配置文件
│           ├── QFluentConfig.cmake
│           ├── QFluentConfigVersion.cmake
│           └── QFluentTargets.cmake
├── include/
│   └── QFluent/              # 头文件
│       ├── FluentGlobal.h
│       ├── Theme.h
│       ├── FluentIcon.h
│       └── ...
└── share/
    └── QFluent/
        └── res/              # 资源文件（图标、样式表）
```

**说明：** Debug 版本的库文件使用 `d` 后缀（如 `QFluentd.dll`、`QFluentd.lib`），遵循 Qt 的命名惯例。这样 Debug 和 Release 版本可以共存于同一目录，互不覆盖。

### 方式二：安装到自定义目录

也可以安装到自定义位置：

```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
cmake --build . --config Release
cmake --install . --config Release
```

然后在项目中指定安装路径：

```cmake
# 方式 1：添加到 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "/path/to/install/lib/cmake/QFluent;${CMAKE_PREFIX_PATH}")
find_package(QFluent REQUIRED)
target_link_libraries(MyApp PRIVATE QFluent::QFluent)
```

或设置 `QFluent_DIR`：

```cmake
# 方式 2：设置 QFluent_DIR
set(QFluent_DIR "/path/to/install/lib/cmake/QFluent")
find_package(QFluent REQUIRED)
target_link_libraries(MyApp PRIVATE QFluent::QFluent)
```

### 方式三：子目录集成

将 QFluentKit 作为子目录添加到项目中：

```cmake
# 添加 QFluentKit 子目录
add_subdirectory(QFluentKit)

# 链接 QFluent
target_link_libraries(MyApp PRIVATE QFluent)
```

### 方式四：手动集成

1. 编译 QFluentKit 生成 `QFluent.dll` (Windows) 或 `libQFluent.so` (Linux/macOS)
2. 在你的项目中包含头文件目录：`QFluentKit/QFluent/src/`
3. 链接生成的库文件

### CMake 选项说明

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `QFLUENT_INSTALL_TO_QT` | OFF | 安装到 Qt 安装目录 |
| `CMAKE_INSTALL_PREFIX` | 系统默认 | 自定义安装路径 |
| `BUILD_QWINDOWKIT` | OFF | 启用 QWindowKit 集成 |

### 可选：启用 QWindowKit 集成

QWindowKit 提供高级窗口管理功能（无边框窗口、毛玻璃效果等）：

```cmake
# 在构建示例程序时启用
set(BUILD_QWINDOWKIT ON CACHE BOOL "Build with QWindowKit support" FORCE)

# 或者通过 CMake 参数启用
cmake -DBUILD_QWINDOWKIT=ON ..
```

启用后，`USE_QWINDOWKIT` 宏会被定义，可使用增强的窗口功能。

---

## 基本用法

### Hello World

```cpp
#include <QWidget>
#include <QApplication>

#include "QFluent/LineEdit.h"
#include "QFluent/PushButton.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建 Fluent 窗口
    QWidget window;
    window.setWindowTitle("QFluentKit 示例");
    window.resize(800, 600);

    // 创建按钮
    auto *button = new PrimaryPushButton(&window);
    button->setText("点击我");
    button->move(350, 280);

    // 创建输入框
    auto *lineEdit = new LineEdit(&window);
    lineEdit->setPlaceholderText("请输入内容...");
    lineEdit->move(350, 340);

    window.show();
    return app.exec();
}

```

---

## 组件列表

### 基础输入
- Button, PrimaryButton, HyperlinkButton
- CheckBox, RadioButton
- ComboBox, LineEdit, Slider, SpinBox
- SwitchButton, PushButton

### 显示组件
- Label, CaptionLabel, StrongLabel
- ImageLabel, IconWidget
- CardWidget, Loading

### 日期时间
- DatePicker, TimePicker
- CalendarPicker, CalendarView
- CycleListWidget

### 菜单导航
- RoundMenu, NavigationPanel, NavigationBar
- Pivot, TabBar

### 对话框
- MessageDialog, ColorDialog, Flyout, TeachingTip

### 容器布局
- StackedWidget, ScrollArea, TableView
- ListView, FlowLayout, ExpandLayout

### 进度状态
- ProgressBar, ProgressRing
- IndeterminateProgressBar, IndeterminateProgressRing
- InfoBar, ToolTip

### 设置卡片
- SettingCard, SettingCardGroup
- ExpandSettingCard, OptionsSettingCard

### 材料效果
- AcrylicWidget, AcrylicLabel, AcrylicMenu, AcrylicToolTip

---

## 项目结构

```
QFluentKit/
├── QFluent/                      # 核心动态库
│   ├── src/
│   │   ├── FluentGlobal.h        # 全局枚举 (ThemeMode, IconType, ThemeStyle)
│   │   ├── Theme.h               # 主题管理系统 (AUTO/LIGHT/DARK)
│   │   ├── Router.h              # 路由系统 (配合 StackedWidget)
│   │   ├── FluentIcon.h          # 图标系统 (248+ 内置 SVG 图标)
│   │   ├── StyleSheet.h          # 样式表管理系统
│   │   ├── Animation.h           # 动画系统基类
│   │   ├── QFluent/              # 公开组件头文件
│   │   │   ├── BasicInput/       # 基础输入组件
│   │   │   ├── Display/          # 显示组件
│   │   │   ├── DateTime/         # 日期时间组件
│   │   │   ├── Menu/             # 菜单组件
│   │   │   ├── Dialog/           # 对话框
│   │   │   ├── Layout/           # 布局容器
│   │   │   ├── Progress/         # 进度组件
│   │   │   ├── Setting/          # 设置卡片
│   │   │   └── Material/         # 材质效果
│   │   └── Private/              # PIMPL 私有实现
│   └── res/                      # 资源文件
│       ├── images/icons/         # Fluent Design SVG 图标
│       └── style/                # QSS 样式表 (light/dark)
├── QFluentExample/                # 示例应用程序
│   ├── src/
│   │   ├── Interface/            # 15 个演示界面
│   │   │   ├── HomeInterface.h
│   │   │   ├── BasicInputInterface.h
│   │   │   └── ...
│   │   └── Window/               # 自定义窗口示例
│   │       ├── LoginWindow.h
│   │       ├── NavbarWindow.h
│   │       └── SplitWindow.h
│   └── libs/qwindowkit/          # 可选窗口管理库
├── CMakeLists.txt                 # 根 CMake 配置
└── README.md
```

---

## 开源协议

本项目采用 [GPLv3](LICENSE) 协议开源。

---

## 贡献

欢迎提交 Issue 和 Pull Request！

---

## 致谢

- 核心窗口管理：[QWindowKit](https://github.com/stdware/qwindowkit)
- 设计灵感来源：[PyQt-Fluent-Widgets](https://github.com/zhiyiYo/PyQt-Fluent-Widgets)

---

<div align="center">

⭐ 如果觉得这个项目对你有帮助，请给个 Star！
🐧 欢迎加入QQ群交流: 1084320682

</div>
