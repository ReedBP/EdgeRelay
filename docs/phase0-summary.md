# Phase 0 总结（工程骨架）

这份文档专门回答两个问题：
1. **Phase 0 主要做了什么？**
2. **这些内容是怎么实现的？**

适合你在面试时做 2~3 分钟的讲解。

---

## 一、Phase 0 主要做了什么

Phase 0 的核心目标是：
**先把“工程基础设施”搭好，确保项目稳定可编译、可运行、可扩展。**

本阶段完成了 4 件事：

1. 建立标准 CMake 工程骨架（`EdgeRelay` + `edge_relay` 可执行文件）。
2. 统一编译规范（C++17、告警选项、可选 warnings-as-errors）。
3. 明确依赖清单（`vcpkg.json`：MQTT/配置/日志/SQLite）。
4. 建立最小 CI（GitHub Actions 在 Windows + MSVC 上自动编译检查）。

一句话总结：
> Phase 0 不是做业务功能，而是把“地基”打牢，让后面 Phase 1/2/3 能平稳推进。

---

## 二、怎么实现的（按模块）

## 1) 工程与构建（CMake）

### 做了什么
- 定义项目：`project(EdgeRelay VERSION 0.1.0 LANGUAGES CXX)`
- 创建可执行文件：`edge_relay`
- 源文件入口迁移到：`src/main.cpp`

### 为什么这样做
- 避免“单文件 demo”难扩展的问题。
- 后续新增 `src/mqtt/`、`src/store/`、`src/source/` 时结构更自然。

### 关键实现点
- C++ 标准固定为 17。
- 同时使用 `target_compile_features(edge_relay PRIVATE cxx_std_17)`，让目标本身声明语言要求。

---

## 2) 编译质量门槛（Warnings 策略）

### 做了什么
- MSVC：`/W4 /permissive-`
- GCC/Clang：`-Wall -Wextra -Wpedantic`
- 新增开关 `EDGERELAY_WARNINGS_AS_ERRORS`（可选开启 `/WX` 或 `-Werror`）

### 为什么这样做
- 尽早暴露潜在问题，减少后续调试成本。
- 兼顾开发体验（默认不强制 Werror）和质量控制（需要时可开启）。

---

## 3) 启动可执行验证（最小运行闭环）

### 做了什么
- `src/main.cpp` 输出固定启动信息：
  `[EdgeRelay] boot mode=phase0-skeleton`

### 为什么这样做
- 给新手和面试官一个非常明确的“跑通信号”。
- 在还未接入 MQTT/SQLite 前，也能先验证工具链和工程配置。

---

## 4) 依赖清单（vcpkg manifest）

### 做了什么
新增 `vcpkg.json`，声明后续阶段将使用：
- `paho-mqttpp3`（MQTT 客户端）
- `tomlplusplus`（TOML 配置）
- `spdlog`（日志）
- `sqlite3`（离线缓冲存储）

### 为什么这样做
- 统一依赖来源，减少“每个人手动装不同版本”的问题。
- 后续接入功能时不需要临时讨论库选型，直接进入实现。

---

## 5) 持续集成（CI）

### 做了什么
新增 `.github/workflows/windows-msvc.yml`：
- 触发条件：push 到 `main/work`，或 PR 到 `main`
- 运行环境：`windows-latest`
- 执行步骤：
  1. checkout
  2. CMake configure（VS2022, x64）
  3. CMake build（Debug）

### 为什么这样做
- 防止“我本地能编译，别人环境不行”。
- 每个 PR 至少保证“能在干净 Windows 环境编译通过”。

---

## 三、Phase 0 完成后的结果

你现在已经有：
1. 一个可运行的 C++ 工程基础骨架。
2. 一套可复用的构建与依赖规范。
3. 一个自动化编译检查流程（CI）。
4. 一份可交付给面试官快速上手的 README。

这意味着：
> **可以安全进入 Phase 1（MQTT 通路）**，并继续按“小步提交、每步可运行”的方式推进。

---

## 四、面试时可以直接说的版本（30 秒）

“Phase 0 我主要做的是工程化打底：
先把 CMake 项目结构、编译规范、依赖清单和 Windows CI 建好，
并用一个最小可执行程序确认环境可跑通。
这样后面做 MQTT/SQLite/OPC UA 时，每次改动都能被自动编译验证，
避免功能还没做完就陷入环境问题。”
