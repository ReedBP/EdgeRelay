# Windows MQTT 验证指令详解（Docker + mosquitto_sub）

这份文档专门解释你截图里的那一套命令：
- 每条命令在做什么
- 为什么要这样写
- 成功后你应该看到什么
- 失败时怎么排查

> 目标：让你不仅“照抄能跑”，还知道每一步在系统设计里扮演什么角色。

---

## 总体思路（先理解流程）

这组命令是在验证 Phase 1 的核心闭环：
1. 启动 MQTT Broker（消息中转站）
2. 用 `mosquitto_sub` 当“监听器”
3. 让 EdgeRelay 发布消息
4. 在监听器里看到消息 => 证明链路打通

也就是：
`EdgeRelay (publisher) -> MQTT Broker -> mosquitto_sub (subscriber)`

---

## 第 1 步：启动 broker

```powershell
docker run -d --name er-mosquitto -p 1883:1883 eclipse-mosquitto:2
```

### 逐段解释
- `docker run`：启动一个容器。
- `-d`：后台运行（detached），不占着当前终端。
- `--name er-mosquitto`：给容器取名，后面 `docker exec` 要用。
- `-p 1883:1883`：把主机 1883 端口映射到容器 1883（MQTT 默认端口）。
- `eclipse-mosquitto:2`：使用 Mosquitto v2 镜像。

### 为什么这样做
你本地不需要手动安装 broker 服务，容器化方式最省事、最可复现。

### 成功标志
- 命令返回一串 container id。
- `docker ps` 能看到 `er-mosquitto` 正在运行。

---

## 第 2 步：订阅消息（开新终端）

```powershell
docker exec -it er-mosquitto mosquitto_sub -h localhost -p 1883 -t edge/stub -v
```

### 逐段解释
- `docker exec -it er-mosquitto ...`：进入运行中的 broker 容器执行命令。
- `mosquitto_sub`：MQTT 订阅客户端。
- `-h localhost -p 1883`：连接容器内部 broker 地址和端口。
- `-t edge/stub`：订阅主题 `edge/stub`。
- `-v`：输出 `topic + payload`，方便观察。

### 为什么要开新终端
这个命令会持续监听，不会自动结束；需要另一个终端去运行 EdgeRelay 发布消息。

### 成功标志
命令执行后“停在那儿等待”，这就是正常的监听状态。

---

## 第 3 步：启用 MQTT 构建并运行 EdgeRelay

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DEDGERELAY_ENABLE_MQTT=ON
cmake --build build --config Debug
.\build\Debug\edge_relay.exe --config configs/edgerelay.toml --sink mqtt --count 3
```

### 第 3.1 行：CMake configure
- `-S . -B build`：源目录是当前目录，构建目录是 `build`。
- `-G "Visual Studio 17 2022" -A x64`：使用 VS2022 64 位生成器。
- `-DCMAKE_TOOLCHAIN_FILE=...vcpkg.cmake`：接入 vcpkg，让 CMake 能找到 Paho 等依赖。
- `-DEDGERELAY_ENABLE_MQTT=ON`：打开项目里的 MQTT 开关（默认是 OFF）。

### 为什么必须开 `EDGERELAY_ENABLE_MQTT`
项目设计成“默认无依赖可运行 + 可选 MQTT 增强”。
不开这个开关时，`--sink mqtt` 会触发受控报错（这是设计行为）。

### 第 3.2 行：编译
- `cmake --build build --config Debug`：编译 Debug 版本。

### 第 3.3 行：运行
- `--config configs/edgerelay.toml`：从配置文件读取默认参数。
- `--sink mqtt`：本次强制使用 MQTT sink（CLI 优先级高于配置文件）。
- `--count 3`：发送 3 条消息后退出，方便验证。

### 成功标志
- 运行终端会打印 `sink=mqtt ...` 和 `done`。
- 第 2 步监听终端会看到 3 行消息（含 topic + JSON payload）。

---

## 常见问题与解决

## 1) `Could not create named generator ...`
说明 `-G` 写了你机器不支持的生成器名称。

处理：
```powershell
cmake --help
```
看 `Generators` 列表，改成你机器可用项（通常是 `Visual Studio 17 2022`）。

## 2) `MQTT support is disabled`
说明 configure 时没加 `-DEDGERELAY_ENABLE_MQTT=ON`，或你用的是旧 build 目录缓存。

处理：
- 重新 configure 并确认开关为 ON。
- 必要时删除 `build` 后重配。

## 3) `mosquitto_sub` 收不到消息
先按顺序检查：
1. broker 是否运行：`docker ps`
2. topic 是否一致：发布和订阅都用 `edge/stub`
3. 程序是否真在 mqtt 模式：运行输出里应有 `sink=mqtt`
4. 端口映射是否正确：`-p 1883:1883`

---

## 你能从这套命令学到什么（面试可讲）

1. **端到端验证方法**：不是“程序能跑就算完”，而是“发布端 + broker + 订阅端”全链路可见。
2. **配置优先级**：`CLI > config file > code default`，这在工程里很常见。
3. **功能开关思想**：`EDGERELAY_ENABLE_MQTT` 让项目兼顾“新手可跑”和“功能可扩展”。
4. **可观测性**：用 `mosquitto_sub -v` 直接观察 topic/payload，排障效率高。

---

## 一键复习（你可以直接照抄）

```powershell
# 1) 启 broker
docker run -d --name er-mosquitto -p 1883:1883 eclipse-mosquitto:2

# 2) 开新终端订阅
docker exec -it er-mosquitto mosquitto_sub -h localhost -p 1883 -t edge/stub -v

# 3) 构建并发布（回到项目终端）
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DEDGERELAY_ENABLE_MQTT=ON
cmake --build build --config Debug
.\build\Debug\edge_relay.exe --config configs/edgerelay.toml --sink mqtt --count 3
```
