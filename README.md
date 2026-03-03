# EdgeRelay

工业风格网关面试项目：把设备数据“中转”出去。
你可以把它理解成一个“快递中转站”：
- 上游数据源（先用模拟数据，后面接 OPC UA）负责“发货”
- EdgeRelay 负责“打包转运”
- MQTT broker 负责“收件分发”
- 如果网络断了，先放到 SQLite（本地小仓库），恢复后再补发

> 当前状态：**Phase 0 / PR 3（CI 自动编译检查）已完成**

---

## 给完全新手的 30 秒说明
- **CMake**：帮你“生成工程文件并编译”的工具。
- **MSVC**：Visual Studio 的 C++ 编译器。
- **MQTT**：轻量消息协议，常用于物联网。
- **SQLite**：一个本地文件数据库，不需要单独安装数据库服务。

你现在只需要先确认：项目能在你电脑上成功编译并运行。后面我们再一步步加功能。

---

## 超详细教程（Windows）

### Step 0：先装好工具
1. 安装 **Visual Studio 2022**，勾选工作负载：`使用 C++ 的桌面开发`。
2. 安装 **Git**。
3. 安装 **CMake**（建议 3.20+）。
4. （可选）安装 **vcpkg**，后续 Phase 1/2/3 要用。

### Step 1：把仓库拉到本地
```powershell
git clone <你的仓库地址>
cd EdgeRelay
```

### Step 2：生成工程文件
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

### Step 3：编译
```powershell
cmake --build build --config Debug
```

### Step 4：运行
```powershell
.\build\Debug\edge_relay.exe
```

### Step 5：看结果
预期输出：
```text
[EdgeRelay] boot mode=phase0-skeleton
```
看到这行 = 你本地环境已经跑通。

---

## 你用的是 VS Code：一步一步点哪里

### Step A：打开 VS Code
1. 打开 **VS Code**。
2. 点击 `File -> Open Folder...`。
3. 选择你刚刚 clone 下来的 `EdgeRelay` 文件夹。

### Step B：第一次打开时，安装这 3 个扩展
在左侧点“扩展”（四个小方块图标），搜索并安装：
1. `C/C++`（Microsoft）
2. `CMake Tools`（Microsoft）
3. `CMake`（twxs）

> 安装完建议重启一次 VS Code。

### Step C：让 VS Code 选择编译器（MSVC）
1. 按 `Ctrl + Shift + P` 打开命令面板。
2. 输入并选择：`CMake: Select a Kit`。
3. 选带有 `Visual Studio 2022 Release - amd64`（或类似名字）的项。

### Step D：配置项目（Configure）
1. 按 `Ctrl + Shift + P`。
2. 输入并选择：`CMake: Configure`。
3. 底部终端出现 `Configuring done` / `Generating done` 就成功。

### Step E：编译项目（Build）
1. 按 `Ctrl + Shift + P`。
2. 输入并选择：`CMake: Build`。
3. 看到 `Built target edge_relay` 就成功。

### Step F：直接运行
你有两种方式：

**方式 1（推荐，最简单）**
- 按 `Ctrl + Shift + P`，执行 `CMake: Run Without Debugging`

**方式 2（终端手动）**
```powershell
.\build\Debug\edge_relay.exe
```

预期输出：
```text
[EdgeRelay] boot mode=phase0-skeleton
```

如果你看到了这行，就表示你已经“在 VS Code 里跑起来了”。

### Step G：如果报错，按这个顺序排查
1. 确认 Visual Studio 2022 装了 `使用 C++ 的桌面开发`。
2. 关闭 VS Code，重新打开项目。
3. 重新执行：`CMake: Select a Kit` -> `CMake: Configure` -> `CMake: Build`。
4. 还不行就删除 `build` 文件夹后再试一次。

---

## （可选但推荐）用 vcpkg 安装后续依赖
> 这一步是给 Phase 1/2/3 做准备。当前程序就算不装这些库也能运行。

```powershell
# 假设你的 vcpkg 在 C:\dev\vcpkg
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

项目依赖已经写在 `vcpkg.json`，后续会自动按清单安装。

---


## 什么是 CI？为什么现在就要加？

### 1) 什么是 CI（小白版）
CI（Continuous Integration，持续集成）你可以理解成：
**“一个自动帮你编译检查的机器人”**。

每次你 push 代码或提 PR，它都会在一台干净的机器上自动执行：
- 拉代码
- 配置 CMake
- 编译项目

### 2) 为什么现在（Phase 0）就要加
现在项目还小，加 CI 成本最低。
等后面接入 MQTT / SQLite / OPC UA 后，改动会越来越多，
这时如果没有 CI，代码很容易“在你电脑能跑，在别人电脑不能跑”。

### 3) 这次 CI 具体做了什么
我们加了 `.github/workflows/windows-msvc.yml`，内容很简单：
1. 使用 `windows-latest` 机器
2. 用 VS2022 生成器执行 `cmake -S . -B build`
3. 执行 `cmake --build build --config Debug`

### 4) 解决了什么问题
- 防止“提交后才发现编译不过”
- 防止“团队/面试官环境和你本地不一致”
- 每个 PR 都有一个最基础的质量门槛（至少能编译）

---

## 面试项目最适配技术选型（直接定案）

### 1) MQTT 库：**Paho MQTT C++**
**为什么选它：**
- 工业项目里使用广，面试官容易认可；
- 示例和资料多，出问题更容易查到答案；
- 支持异步连接/发布，后面做重连与补发更顺。

### 2) 配置格式：**TOML（toml++）**
**为什么选它：**
- 比 JSON 更像“配置文件”，注释友好、可读性高；
- 你给面试官展示配置时更直观；
- toml++ 是现代 C++ 库，集成成本低。

### 3) 日志库：**spdlog**
**为什么选它：**
- 上手非常快，几行代码就能打日志；
- 性能好、功能全（控制台/文件/格式化）；
- 社区常见，面试官熟悉。

> 这三项组合的核心目标：**稳定、资料多、容易讲清楚、实现速度快**。

---

## 当前目录结构
```text
.
├─ CMakeLists.txt
├─ README.md
├─ docs/
│  └─ plan.md
├─ src/
│  └─ main.cpp
└─ vcpkg.json
```

## 分阶段路线图（摘要）
- Phase 0：工程骨架（CMake/vcpkg/log/config/CI）
- Phase 1：MQTT 通路（发布到 mosquitto，`mosquitto_sub` 可见）
- Phase 2：离线缓冲（SQLite 落库 + 重连补发）
- Phase 3：OPC UA 接入（先 stub，后 open62541）

详见 `docs/plan.md`。
