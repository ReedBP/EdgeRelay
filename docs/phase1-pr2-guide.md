# Phase 1 / PR2 讲解：运行参数 + 可切换 Sink（Console/MQTT）

这份文档专门回答你这几个问题：
1. `./build/edge_relay --count 2` 和 `./build/edge_relay --sink mqtt --count 1` 是什么？为什么这么写？
2. 新增模块是怎么做的？为什么这么做？解决了什么问题？
3. 你能学到什么？怎么通过练习把“整体设计”吃透？

---

## 一、两条测试命令到底在测什么？

> 注意：下面命令是 Linux/macOS 写法；Windows PowerShell 用 `./build/Debug/edge_relay.exe ...`。

### 命令 1
```bash
./build/edge_relay --count 2
```
- **作用**：跑“控制台模式”并发送 2 条模拟数据。
- **为什么这么写**：
  - `--count 2` 让程序只跑两次，方便快速验证，不用等很久。
  - 没写 `--sink` 时，默认就是 `console`，也就是打印到终端。
- **你应该看到什么**：
  - 一行 `sink=console`
  - 2 条 JSON 消息
  - 一行 `done`

### 命令 2
```bash
./build/edge_relay --sink mqtt --count 1
```
- **作用**：试图切换到 MQTT 模式并发送 1 条消息。
- **为什么这么写**：
  - `--sink mqtt` 明确告诉程序“输出目标改成 MQTT broker”。
  - `--count 1` 只发一条，做最小验证。
- **当前默认行为**：
  - 因为默认编译没有开 `EDGERELAY_ENABLE_MQTT`，程序会给出“MQTT 未启用”的明确错误。
  - 这个“失败”是我们故意设计的**可预期失败**，它能证明参数解析和保护逻辑是生效的。

---

## 二、本次新增了哪些模块？每个模块做什么？

## 1) `src/runtime_config.h/.cpp`
- **用途**：把命令行参数解析成结构体 `RuntimeConfig`。
- **实现方式**：
  - `parse_runtime_config(argc, argv)` 逐个扫描参数。
  - 支持 `--sink`、`--count`、`--mqtt-server`、`--mqtt-client-id`、`--mqtt-topic`。
  - 参数不合法就抛异常，主函数统一捕获并输出友好错误。
- **解决问题**：
  - 不再需要“改代码+重编译”才能换模式；直接改运行命令即可。

## 2) `src/mqtt_sink.h/.cpp`
- **用途**：新增真正的 MQTT 发布端（实现 `MessageSink` 接口）。
- **实现方式**：
  - 构造函数里连接 broker。
  - `publish()` 把 `SensorMessage` 转 JSON 后发布，QoS=1。
  - 析构时断开连接，尽量优雅退出。
  - 通过 `#ifdef EDGERELAY_ENABLE_MQTT` 做编译期开关。
- **解决问题**：
  - 让项目具备“真实 MQTT 输出能力”，同时不影响没装依赖的同学先跑通。

## 3) `src/pipeline.h/.cpp`（改造）
- **用途**：把原来“固定写死 ConsoleSink”改成“接收任意 `MessageSink&`”。
- **实现方式**：
  - 新签名：`run_pipeline(std::size_t count, MessageSink& sink)`。
  - 主流程只负责“读 source -> 调 sink.publish()”。
- **解决问题**：
  - 主流程和具体输出实现解耦。
  - 后续加 SQLite 离线缓冲时，改动范围更小。

## 4) `src/main.cpp`（改造）
- **用途**：作为“组装层”（composition root）。
- **实现方式**：
  - 先解析参数得到 `RuntimeConfig`。
  - 根据 `sink_mode` 创建 `ConsoleSink` 或 `MqttSink`。
  - 调 `run_pipeline(config.count, *sink)` 执行。
- **解决问题**：
  - 业务流程代码更清晰：配置、组装、执行分离。

## 5) `CMakeLists.txt`（改造）
- **用途**：把新模块加入构建并新增 MQTT 选项。
- **实现方式**：
  - 新增 `option(EDGERELAY_ENABLE_MQTT OFF)`。
  - ON 时 `find_package(eclipse-paho-mqtt-cpp)` 并链接 `paho-mqttpp3`。
- **解决问题**：
  - 保持“默认可编译”，同时提供“可选增强”。

---

## 三、为什么这套设计是面试项目最适合的？

### 1) 先解耦，再接真实依赖
- 你先理解系统结构（source / pipeline / sink）。
- 然后只替换 sink，就能扩展 MQTT / SQLite。
- 这是最容易讲给面试官听的“工程化思路”。

### 2) 把失败变成可解释的行为
- `--sink mqtt` 在未启用时明确报错，不是“莫名其妙崩溃”。
- 面试时你可以说：
  - “我保留了默认无依赖可运行路径，降低上手门槛；
  - 依赖准备好后再打开增强能力。”

### 3) 主函数只做“装配”，不做业务细节
- 这是可维护代码的核心习惯。
- 后续 Phase 2/3 功能再多，`main` 仍保持清晰。

---

## 四、你这一步能学到什么（重点能力）

1. **接口抽象**：`MessageSink` 让实现可替换。
2. **依赖隔离**：第三方库（Paho）被收敛在 `MqttSink` 内部。
3. **配置驱动**：运行参数驱动行为，而不是改源码。
4. **可预期错误处理**：把错误变成明确提示，而非隐式失败。
5. **小步可运行演进**：每一小步都可编译、可验证。

---

## 五、练习清单（按难度递进）

### 练习 1：基础运行
```bash
./build/edge_relay --count 2
```
目标：理解默认模式是 console，`count` 影响循环次数。

### 练习 2：故意输错参数
```bash
./build/edge_relay --sink abc
```
目标：观察错误信息，理解参数校验。

### 练习 3：改默认主题
- 修改 `RuntimeConfig` 里的 `mqtt_topic` 默认值。
- 重新编译运行，确认行为变化。
目标：理解“默认配置”的作用。

### 练习 4：把发送间隔改成 500ms
- 修改 `pipeline.cpp` 的 `sleep_for`。
目标：体验节奏控制与运行行为变化。

### 练习 5：真正连 MQTT（可选进阶）
1. 用 vcpkg + toolchain 重新 CMake，并启用 `-DEDGERELAY_ENABLE_MQTT=ON`。
2. 启动 mosquitto。
3. 用 `mosquitto_sub` 订阅主题。
4. 运行：
   ```bash
   ./build/edge_relay --sink mqtt --count 3 --mqtt-server tcp://localhost:1883 --mqtt-topic edge/stub
   ```
目标：打通端到端真实链路。

---

## 六、下一步（Phase 1 / PR3）
下一步会做：
- 增加 `configs/edgerelay.toml`（从“参数驱动”升级到“配置文件驱动”）。
- 给出 Docker 启动 mosquitto + `mosquitto_sub` 验证脚本。
- 让你不改代码就能完整演示“发布 -> 订阅可见”。
