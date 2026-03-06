# Phase 1 / PR3 讲解：配置文件驱动 + mosquitto 可视化验证

你这一步要求的是：
- 新增模块“怎么做、为什么这么做、解决什么问题、你能学到什么”
- 还要有练习，帮你理解整体软件设计

这份文档就是按这个目标写的。

---

## 1) 本次新增模块总览

### `configs/edgerelay.toml`
- **用途**：放默认运行配置（sink、count、broker、topic）。
- **为什么新增**：减少“每次启动都敲一长串参数”。
- **解决问题**：不同环境（本机/测试机）可以只改配置文件，不改代码。

### `src/config_file.h/.cpp`
- **用途**：把 `edgerelay.toml` 读取到 `RuntimeConfig`。
- **实现方式（核心）**：
  1. 逐行读取；
  2. 去掉注释和空白；
  3. 识别 section（`[app]`、`[mqtt]`）；
  4. 解析 `key=value` 并写入对应字段；
  5. 未知键直接报错。
- **解决问题**：配置错误会被尽早发现，不会“默默忽略”。

### `src/runtime_config.cpp`（升级）
- **新增能力**：支持 `--config <path>`。
- **实现策略**：两次扫描参数：
  - 第一次先加载配置文件；
  - 第二次处理 CLI，让 CLI 覆盖配置文件。
- **解决问题**：同时拥有“稳定默认配置”与“临时快速覆盖”的能力。

---

## 2) 为什么这样设计（背后的工程思路）

### 设计原则 A：稳定默认 + 灵活覆盖
- 配置文件给出“团队通用默认值”。
- 命令行用于“本次调试临时覆盖”。
- 这就是很多生产系统常见的优先级策略：`CLI > Config File > Code Default`。

### 设计原则 B：主流程不依赖配置细节
- `main` 只拿到一个 `RuntimeConfig`，然后组装 sink 并运行 pipeline。
- pipeline 不需要知道配置来源（CLI 还是文件）。
- 好处：职责清晰，后续扩展（比如环境变量）更容易接入。

### 设计原则 C：错误早失败（Fail Fast）
- 未知字段、坏格式、非法值都立刻报错。
- 比“悄悄忽略错误配置”更安全，也更好排查。

---

## 3) 这一步你能学到什么

1. **配置优先级设计**：为什么 CLI 要覆盖文件。
2. **模块分层**：配置读取层、组装层、业务执行层分离。
3. **可观测性**：通过 `mosquitto_sub` 看到真实消息，形成闭环验证。
4. **最小改动扩展**：新增配置能力时，不需要推翻 pipeline 与 sink 抽象。

---

## 4) 实操练习（按顺序）

## 练习 1：纯配置文件运行（console）
1. 把 `configs/edgerelay.toml` 中 `sink="console"`。
2. 运行：
```powershell
.\build\Debug\edge_relay.exe --config configs/edgerelay.toml
```
3. 观察：应打印 JSON 消息。

**你学到**：程序已能从配置文件启动，不依赖长命令参数。

## 练习 2：CLI 覆盖配置文件
1. 保持配置里 `sink="console"`。
2. 运行：
```powershell
.\build\Debug\edge_relay.exe --config configs/edgerelay.toml --count 2
```
3. 观察：只发送 2 条。

**你学到**：CLI 确实覆盖了配置文件值。

## 练习 3：MQTT 闭环验证（重点）
1. 启动 mosquitto（Docker）。
2. 在另一个终端执行 `mosquitto_sub` 订阅 `edge/stub`。
3. 用 `--sink mqtt --count 3` 启动程序。
4. 观察订阅端是否收到 3 条消息。

**你学到**：系统从“本地打印”升级到“网络消息发布”，Phase 1 主目标达成。

## 练习 4：改主题，验证路由
1. 把配置文件 topic 改成 `edge/demo2`。
2. 订阅改成 `edge/demo2`。
3. 运行程序，看是否正常收到。

**你学到**：topic 是“消息路由地址”，决定谁能收到消息。

---

## 5) 常见坑与排查

1. **`--sink mqtt` 报 MQTT disabled**
   - 原因：编译时没开 `-DEDGERELAY_ENABLE_MQTT=ON`。
2. **连接 broker 失败**
   - 原因：mosquitto 没起、端口没映射、地址写错。
3. **`mosquitto_sub` 收不到**
   - 原因：topic 不一致（最常见）。
4. **配置文件解析错误**
   - 原因：键名拼错，或 section 写错（比如 `[mtqq]`）。

---

## 6) 这一步和 Phase 2 的关系

你现在已经具备：
- 可切换 sink
- 可配置运行参数
- 可验证 MQTT 发送

这三个条件刚好是 Phase 2（离线缓冲）的地基：
下一步只要新增 SQLite 缓冲 sink，就能把“发送失败时落盘”接进现有架构，而不重写主流程。
