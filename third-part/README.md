# 第三方库构建与安装

本目录使用一个独立的 CMake Superbuild 编排本地第三方源码：

- `fmt`
- `yaml-cpp`

两个项目分别配置、编译和安装，不共享构建目录，也不共享安装前缀。

## 环境要求

- CMake 3.25+
- MinGW 和 `mingw32-make`，或带 C++ 工具链的 Visual Studio 2026

如果使用 Ninja 等其他生成器，可以在本目录创建不提交版本库的
`CMakeUserPresets.json`，继承现有配置并覆盖 `generator`。

## 一条命令构建并安装

在 `third-part` 目录执行：

```powershell
cd E:\c++\CMake_learn\third-part
cmake --workflow --preset release-static
```

可用组合：

| Workflow Preset | 构建类型 | 库类型 |
|---|---|---|
| `debug-static` | Debug | 静态库 |
| `debug-shared` | Debug | 动态库 |
| `release-static` | Release | 静态库 |
| `release-shared` | Release | 动态库 |

以上四项使用 MinGW。Visual Studio 2026 x64 对应的 Preset 为：

| Workflow Preset | 构建类型 | 库类型 |
|---|---|---|
| `vs-debug-static` | Debug | 静态库 |
| `vs-debug-shared` | Debug | 动态库 |
| `vs-release-static` | Release | 静态库 |
| `vs-release-shared` | Release | 动态库 |

例如使用 Visual Studio 编译 Release 静态库：

```powershell
cmake --workflow --preset vs-release-static
```

Visual Studio 是多配置生成器，相关 Build Preset 已通过 `configuration`
选择正确的 Debug 或 Release 配置，不需要手工添加 `--config`。

Workflow 会完成配置、编译和安装。两个库使用完全独立的目录：

```text
out/
├── build/third-part/
│   ├── fmt/<preset-name>
│   ├── yaml-cpp/<preset-name>
│   └── superbuild/<preset-name>
└── install/third-part/
    ├── fmt/<preset-name>
    └── yaml-cpp/<preset-name>
```

`superbuild` 目录只保存编排信息，不包含 fmt 或 yaml-cpp 的编译产物。

## 分步执行

以 Release 静态库为例：

```powershell
cmake --preset release-static
cmake --build --preset release-static
```

`ExternalProject` 的构建目标本身包含安装步骤，因此无需再手工执行
`--target install`。

## 只构建其中一个库

先配置对应 Preset，然后指定 Superbuild 目标：

```powershell
# 只构建并安装 fmt
cmake --preset release-static
cmake --build --preset release-static --target fmt

# 只构建并安装 yaml-cpp
cmake --build --preset release-static --target yaml-cpp
```

## 在其他 CMake 工程中使用

将两个独立安装前缀加入 `CMAKE_PREFIX_PATH`：

```powershell
cmake -S . -B build `
  -DCMAKE_PREFIX_PATH="E:\c++\CMake_learn\out\install\third-part\fmt\release-static;E:\c++\CMake_learn\out\install\third-part\yaml-cpp\release-static"
```

然后正常查找并链接：

```cmake
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(
    your_target
    PRIVATE
        fmt::fmt
        yaml-cpp::yaml-cpp
)
```

如果使用动态库，运行程序时还需要让 Windows 能找到安装目录 `bin` 下的 DLL：

```powershell
$env:PATH = "E:\c++\CMake_learn\out\install\third-part\fmt\release-shared\bin;E:\c++\CMake_learn\out\install\third-part\yaml-cpp\release-shared\bin;$env:PATH"
```

## 查看可用 Preset

```powershell
cmake --list-presets
cmake --list-presets=workflow
```

## 常用 CMake 选项

Superbuild 已默认关闭第三方项目自身的测试、文档和工具，只构建并安装库：

```text
FMT_TEST=OFF
FMT_DOC=OFF
YAML_CPP_BUILD_TESTS=OFF
YAML_CPP_BUILD_TOOLS=OFF
```

这样能缩短第三方依赖的构建时间；MyLib 和 Consumer 的测试仍由各自工程负责。

## 本地兼容补丁

当前仓库中的 yaml-cpp 0.9.0 为兼容 MinGW GCC 6.3，对
`src/contrib/dragonbox.h` 中两个类内静态函数调用增加了 `impl::` 限定。
该修改只解决旧 GCC 的模板名称查找问题，不改变运行逻辑。升级第三方源码时应
检查上游是否已包含等效修复。
