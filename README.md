# zhou0bin

## 目录作用说明

本仓库当前主要由 `remote_cod` 目录及其多个子模块组成，整体看起来是一个基于 CMake 的诊断相关工程集合（含脚本解析、诊断代理、虚拟测试器、DoIP 边缘模块等）。

### 顶层目录

- `remote_cod/`：核心源码目录，包含多个相互独立或可组合的 CMake 子工程。
- `scripts/`：仓库辅助脚本目录，当前提供一键编译脚本 `build_all.sh`。
- `.git/`：Git 版本管理元数据目录（提交历史、分支引用、钩子等），不属于业务代码。

### `remote_cod` 下各文件夹作用

- `remote_cod/diag_script_parser/`：诊断脚本解析相关模块目录，目前包含 `.gitignore`，通常用于放置解析器源码或构建产物过滤规则。
- `remote_cod/AsfDiagnosticScript/`：`AsfDiagnosticScript` 工程目录，包含 `AsfDiagnosticScript.cpp` 与 `CMakeLists.txt`，用于构建诊断脚本相关可执行程序或库。
- `remote_cod/AsfVirtualDiagnosticTester/`：`AsfVirtualDiagnosticTester` 工程目录，包含测试器实现与 CMake 配置，面向虚拟化诊断测试场景。
- `remote_cod/uds_inf/`：UDS（Unified Diagnostic Services）接口/基础能力目录，含 `CMakeLists.txt` 与 `.gitignore`，一般用于承载 UDS 通信或接口抽象。
- `remote_cod/AsfDiagnosticProxy/`：诊断代理模块目录，包含代理实现源码和构建脚本，通常用于转发/桥接诊断请求。
- `remote_cod/diag_doip_edge/`：DoIP（Diagnostics over IP）边缘模块目录，含 CMake 配置和忽略规则，通常用于以太网诊断接入边缘能力。
- `remote_cod/virt_diag_tester/`：虚拟诊断测试器目录，目前以 `CMakeLists.txt` 为主，可作为测试程序或测试框架入口。

## 编译说明

### 环境要求

- CMake（建议 3.2+，以各子模块 `CMakeLists.txt` 为准）
- C/C++ 编译器（如 `gcc/g++` 或 `clang/clang++`）
- `make` 或 Ninja（取决于 CMake 生成器）

### 一键编译（推荐）

在仓库根目录执行：

```bash
bash scripts/build_all.sh
```

脚本行为：

- 自动扫描 `remote_cod/*` 下包含 `CMakeLists.txt` 的模块；
- 为每个模块单独在 `build/<模块名>/` 目录配置并编译；
- 最后输出成功/失败汇总，若有失败模块则脚本返回非 0。

可选环境变量：

```bash
BUILD_TYPE=Debug GENERATOR="Ninja" bash scripts/build_all.sh
```

- `BUILD_TYPE`：默认 `Release`
- `GENERATOR`：默认使用 CMake 默认生成器

### 单模块编译示例

```bash
cmake -S remote_cod/AsfDiagnosticScript -B build/AsfDiagnosticScript -DCMAKE_BUILD_TYPE=Release
cmake --build build/AsfDiagnosticScript -j"$(nproc)"
```

### 常见失败原因

如果你在本地直接编译失败，通常是以下原因：

- 缺少私有依赖包（例如 `neusar_net`、`log_wrapper`）；
- 缺少第三方依赖（例如 Boost 开发包）；
- 缺少头文件生成物（如 `midware/diag/virtual_diagnostics.h` 对应的上游产物未准备）。

可通过设置 `CMAKE_PREFIX_PATH` 或 `<PackageName>_DIR` 指向已安装依赖的位置后重新执行编译。
