# MinArch Android

基于 [MinUI](https://github.com/shauninman/MinUI) 的 minarch 引擎的 Android 移植版本。

## 项目概述

MinArch 是一个 libretro 前端，最初为复古掌机设计。本项目将其移植到 Android 平台，使用原生 Android API (EGL/OpenGL ES)，无需 SDL 依赖。

## 技术特点

- **原生图形**: 使用 EGL + OpenGL ES 2.0 渲染
- **轻量级**: 无 SDL2 依赖，直接使用 Android Native API
- **libretro**: 支持加载 libretro 核心 (.so)
- **虚拟手柄**: 支持触摸屏虚拟按键
- **核心管理**: 内置核心管理和游戏库

## 项目结构

```
minarch_android/
├── app/
│   ├── src/main/
│   │   ├── java/com/minarch/
│   │   │   ├── MainActivity.kt         # 主界面
│   │   │   ├── MinArchNative.kt        # JNI 包装
│   │   │   ├── VirtualGamepadView.kt   # 虚拟手柄
│   │   │   ├── CoreManager.kt          # 核心管理
│   │   │   ├── GameManager.kt          # 游戏库管理
│   │   │   └── SettingsManager.kt      # 设置管理
│   │   ├── jni/
│   │   │   ├── minarch/
│   │   │   │   ├── jni_entry.c         # JNI 入口
│   │   │   │   ├── minarch_android.c   # 核心实现
│   │   │   │   ├── minarch.c           # 原始源码
│   │   │   │   └── libretro.h          # libretro API
│   │   │   ├── common/                 # 共享代码
│   │   │   └── platform/               # Android 平台层
│   │   └── res/                        # 资源文件
│   └── build.gradle
├── build.gradle
├── settings.gradle
└── gradle.properties
```

## 构建

1. 打开 Android Studio
2. 导入项目
3. 构建 Debug 或 Release APK

```bash
./gradlew assembleDebug
```

## 使用

1. 安装 APK 到设备
2. 选择一个 libretro 核心 (.so 文件)
3. 选择一个 ROM 文件
4. 开始游戏

### 操控方式

- **物理手柄**: 自动检测并使用
- **虚拟手柄**: 触摸屏显示 D-pad 和动作按键
- **键盘**: 方向键 + Z/X/A/S 等

### 支持的系统

| 系统 | 扩展名 | 推荐核心 |
|------|--------|----------|
| NES | nes, nez, unf | FCEUmm |
| GB/GBC | gb, gbc | Gambatte |
| GBA | gba | mGBA |
| SNES | sfc, smc, swc | Snes9x |
| Genesis | md, gen | Genesis Plus GX |
| PlayStation | bin, cue, iso | PCSX Rearmed |
| N64 | n64, z64 | Mupen64Plus |
| NDS | nds | DeSmuME |

## 依赖

- Android NDK (推荐 r21 或更高版本)
- Android SDK (API 21+)
- OpenGL ES 2.0

## 许可证

MIT License - 继承 MinUI 许可证

## 参考

- [MinUI](https://github.com/shauninman/MinUI)
- [libretro](https://github.com/libretro/libretro-common)