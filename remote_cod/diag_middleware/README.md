# diag_middleware

一个可配置的诊断中间件样例，用于接收/处理远端下发的诊断消息，并执行诊断任务（如读/清 DTC、按 ECU 刷写、整车刷写）。

## 支持的任务

- `ACTION=READ_DTC;ECU=<ecu>`
- `ACTION=CLEAR_DTC;ECU=<ecu>`
- `ACTION=FLASH_ECU;ECU=<ecu>;IMAGE=<path>`
- `ACTION=FLASH_ALL;IMAGE=<path>`

## 配置文件

默认配置文件：`conf/middleware.conf`

- `ALLOW_REMOTE_FLASH=true|false`：是否允许远端触发刷写。
- `ALLOW_FLASH_ALL=true|false`：是否允许整车刷写。
- `ECU_WHITELIST=BCM,TCU,...`：允许被操作的 ECU 白名单（留空则允许全部）。

## 编译运行

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
./build/diag_middleware --config conf/middleware.conf --inbox conf/sample_messages.txt
```

也可直接用脚本：

```bash
bash scripts/run_with_sample.sh
```
