# QFluentKit

<div align="center">

一款美观的现代 Qt Fluent Design 组件库，提供 90+ 开箱即用的高质量组件

[English](#) · **简体中文**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5.12%2B-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/Zhenyi-Zhou/QFluentKit)

</div>

---

## 简介

**QFluentKit** 是一个基于 Qt 的 Fluent Design 风格组件库，提供完整的现代化 UI 组件集合。项目采用 C++17 标准，支持 Qt5/Qt6 双版本，兼容 Windows、Linux 和 macOS 平台。

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
git clone https://github.com/Zhenyi-Zhou/QFluentKit.git
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

### CMake 方式

在你的 `CMakeLists.txt` 中添加：

```cmake
# 设置 QFluentKit 路径
set(QFluentKit_DIR "path/to/QFluentKit")

# 查找 QFluentKit 包
find_package(QFluentKit REQUIRED)

# 链接库
add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE QFluentKit)
```

### 手动方式

1. 编译生成 `QFluentKit.dll` / `libQFluentKit.so`
2. 在你的项目中包含头文件目录
3. 链接生成的库文件

---

## 基本用法

### Hello World

```cpp
#include <QApplication>
#include <Fluent/Window/FluentWidget>
#include <Fluent/Widgets/Button>
#include <Fluent/Widgets/LineEdit>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建 Fluent 窗口
    Fluent::FluentWidget window;
    window.setWindowTitle("QFluentKit 示例");
    window.resize(800, 600);

    // 创建按钮
    auto *button = new Fluent::PrimaryButton(&window);
    button->setText("点击我");
    button->move(350, 280);

    // 创建输入框
    auto *lineEdit = new Fluent::LineEdit(&window);
    lineEdit->setPlaceholderText("请输入内容...");
    lineEdit->move(350, 340);

    window.show();
    return app.exec();
}
```

### 主题切换

```cpp
#include <Fluent/Core/Theme>

// 设置深色主题
Fluent::Theme::instance()->setTheme(Fluent::ThemeMode::DARK);

// 设置自动主题（跟随系统）
Fluent::Theme::instance()->setTheme(Fluent::ThemeMode::AUTO);

// 自定义主题色
Fluent::Theme::instance()->setThemeColor(QColor("#0078D4"));
```

### 使用图标

```cpp
#include <Fluent/Icon/FluentIcon>

auto *iconWidget = new Fluent::IconWidget(&window);
iconWidget->setIcon(Fluent::FluentIcon::HOME);
iconWidget->setIconSize(QSize(32, 32));
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
├── QFluent/              # 核心库
│   ├── src/
│   │   ├── FluentGlobal.h    # 全局枚举和宏定义
│   │   ├── QFluent/          # 公开组件头文件
│   │   ├── Private/          # 私有实现
│   │   └── Core/             # 核心系统 (Theme, Router, Animation)
│   └── res/               # 资源文件 (图标、样式表)
├── QFluentExample/        # 示例程序
│   └── src/               # 演示界面
└── CMakeLists.txt         # 根 CMake 配置
```

---

## 开源协议

本项目采用 [MIT](LICENSE) 协议开源。

---

## 贡献

欢迎提交 Issue 和 Pull Request！

---

## 致谢

- 设计灵感来源于 [Microsoft Fluent Design System](https://www.microsoft.com/design/fluent/)
- 图标资源来自 [Fluent UI System Icons](https://developer.microsoft.com/en-us/fluentui)

---

<div align="center">

⭐ 如果觉得这个项目对你有帮助，请给个 Star！

</div>
