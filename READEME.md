# CMake 多级 Package 示例

## 从根目录构建全部工程

在仓库根目录执行：

```powershell
cmake --workflow --preset dev
```

总 Preset 会自动编排依赖关系：

```text
upstream ─┐
          ├──> mylib ──> consumer
fmt ──────┘
```

Upstream 和 fmt 都安装完成后才会配置 MyLib；MyLib 安装完成后才会配置
Consumer。每一级仍是独立 CMake 工程，并通过安装后的 package 连接。

Release 构建使用：

```powershell
cmake --workflow --preset release
```

本仓库包含三个完全独立的 CMake 工程，依赖关系如下：

```text
upstream ──> mylib ──> consumer
```

- **upstream**：模拟 MyLib 所需的第三方依赖，安装并导出 `Upstream` package。
- **mylib**：使用 `find_package(Upstream)`，安装并导出 `MyLib` package。
- **consumer**：只直接查找并链接 `MyLib`；`MyLib` 自动查找其 `Upstream` 依赖。

每一层都只使用上一层的安装产物，不会引用上一层源码目录。生成物统一放在 `out/`，不会污染源码树。

## 环境要求

- CMake 3.25+（使用 Workflow Presets）
- 支持 C++11 的编译器
- MinGW（当前 Presets 默认使用 `MinGW Makefiles`）// 可配置

使用 Ninja、Visual Studio 或其他工具链时，可以在不提交版本库的 `CMakeUserPresets.json` 中继承 Preset 并覆盖 `generator`。

## 1. 构建并安装 Upstream

```powershell
cd upstream
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
cmake --build --preset dev --target install
```

安装位置为 `out/install/upstream`。

以上四条命令也可以简化为：

```powershell
cmake --workflow --preset dev
```

## 2. 构建并安装 MyLib

```powershell
cd ../mylib
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
cmake --build --preset dev --target install
```

MyLib Preset 的 `CMAKE_PREFIX_PATH` 指向已安装的 Upstream。安装位置为 `out/install/mylib`。

也可以用一条命令完成：

```powershell
cmake --workflow --preset dev
```

## 3. 构建并运行 Consumer

```powershell
cd ../consumer
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
.\..\out\build\consumer\dev\consumer.exe
```

前三条命令也可以简化为：

```powershell
cmake --workflow --preset dev
```

Consumer 的 `CMAKE_PREFIX_PATH` 同时包含 MyLib 和 Upstream 的安装前缀。虽然 Consumer 只调用 `find_package(MyLib)`，但 `MyLibConfig.cmake` 中的 `find_dependency(Upstream)` 会解析传递依赖。

## 关键 CMake 写法

MyLib 查找并公开链接额外依赖：

```cmake
find_package(Upstream 1.0 CONFIG REQUIRED)
target_link_libraries(mylib PUBLIC Upstream::upstream)
```

`MyLibConfig.cmake.in` 为使用者恢复传递依赖：

```cmake
include(CMakeFindDependencyMacro)
find_dependency(Upstream 1.0 CONFIG)
include("${CMAKE_CURRENT_LIST_DIR}/MyLibTargets.cmake")
```

消费者只需要：

```cmake
find_package(MyLib 1.0 CONFIG REQUIRED)
target_link_libraries(consumer PRIVATE MyLib::mylib)
```

## Release 构建

将每一步的 `dev` 换成 `release` 即可。Debug 和 Release 使用不同的构建目录。

## 关于安装布局

示例将 Upstream 和 MyLib 安装到两个前缀，以清楚展示依赖查找过程。实际发布时，也可以把它们安装到同一个公共前缀，此时消费者只需提供一个 `CMAKE_PREFIX_PATH`。
