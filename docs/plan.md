# EdgeRelay 迭代计划（小白友好版）

下面这份计划按“每次只做一小步、每一步都能跑”的原则设计。
你可以把它当作“施工清单”：按顺序做，不会乱。

---

## Phase 0：工程骨架（已完成）

### 目标（做完后你会得到什么）
1. 一个在 Windows + VS Code + MSVC 下稳定可编译的 C++ 项目。
2. 统一项目规范：依赖管理、日志、配置、CI。
3. 给后面 MQTT / SQLite / OPC UA 打地基。

### 预计改动文件（拆成多个小提交）
- PR1（已完成）：`CMakeLists.txt`、`src/main.cpp`、`README.md`、`docs/plan.md`
- PR2（已完成）：`vcpkg.json`（声明依赖：paho-mqttpp3、tomlplusplus、spdlog、sqlite3）
- PR3（已完成）：`.github/workflows/windows-msvc.yml`（最小 CI，只做 configure+build）

### 你需要掌握的点（白话）
- CMake 是“帮你生成工程和编译”的。
- vcpkg 是“帮你下载第三方库并和 CMake 对接”的。
- CI 是“每次提交后自动帮你编译检查”的机器人。

---

## Phase 1：MQTT 通路（先把消息发出去）

### 目标
1. 程序每隔几秒发一条测试消息到 MQTT。
2. 你在终端里用 `mosquitto_sub` 能看到消息。

### 预计改动文件
- `src/mqtt/*`（MQTT 客户端封装）
- `src/app/*`（主流程）
- `configs/edgerelay.toml`（broker 地址、主题、发送间隔）
- `README.md`（Docker 启动 mosquitto + 验证步骤）

### 你需要掌握的点
- QoS：消息“尽量到达”的等级。
- reconnect：断线后自动重连。
- 封装思想：业务代码不要直接依赖第三方库细节。

---

## Phase 2：离线缓冲（网络断了也不丢）

### 目标
1. broker 不可用时，把消息存到 SQLite。
2. broker 恢复后按顺序补发。
3. 补发成功后标记为已发送。

### 预计改动文件
- `src/store/*`（SQLite 存取）
- `sql/schema.sql`（数据表结构）
- `src/app/flush_worker.*`（补发线程/任务）

### 你需要掌握的点
- 事务：保证写入可靠。
- at-least-once：宁可重复，不可丢失。
- 幂等：重复发送时，下游也能安全处理。

---

## Phase 3：OPC UA 接入（从模拟数据到真实工业协议）

### 目标
1. 保留 stub 数据源，新增 OPC UA 数据源。
2. 统一成同一种内部数据结构，再发到 MQTT。

### 预计改动文件
- `src/source/stub_*`
- `src/source/opcua_*`

### 你需要掌握的点
- OPC UA 订阅模型。
- 采样周期、时间戳、质量码。
- 抽象接口（让 stub 和 opcua 可替换）。

---

## 技术选型（面试项目定案）

### MQTT：Paho MQTT C++
- **结论**：选它。
- **原因**：成熟、资料多、面试官认可度高、后续重连/异步更容易实现。

### 配置：TOML（toml++）
- **结论**：选它。
- **原因**：配置可读性高、展示友好、比 YAML 更不容易踩语法坑。

### 日志：spdlog
- **结论**：选它。
- **原因**：上手快、功能够用、性能好、社区常见。

> 这套组合不是“最炫”，而是“面试最稳妥”：
> **实现速度快 + 可解释性强 + 出问题好排查**。
