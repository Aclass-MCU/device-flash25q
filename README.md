# device-flash25q

25q 系列 SPI / QSPI Nor Flash 驱动。

## 已知限制

### 单 QSPI 实例限制

当前 `sfud_port.c` 使用全局变量 `g_qspi` 持有 QSPI handle，
SFUD 回调直接引用该变量，不支持多片 Flash 并发操作。

**表现：** 系统中只能初始化一个 `aDevFlash25qHandle_t` 实例。
第二个实例初始化时会覆盖 `g_qspi`，导致先前实例的 SFUD 操作指向错误的 QSPI 外设。

**改进方向：**

端口层引入注册表映射 `sfud_flash* → aDrvQspiHandle_t*`，
在 `spi_write_read` / `qspi_read` 回调中通过 `container_of(spi, sfud_flash, spi)`
获取当前 Flash 设备，再查表找到对应的 `aDrvQspiHandle_t*`。

涉及变更：
- `sfud_port.c` — 新增映射数组 + 注册/查找接口，去除全局 `g_qspi`
- `aDev_flash25q.c` — 初始化成功后调用注册接口关联 `sfud_dev` 与 `qspi`
- SFUD 库源码（`sfud/inc/*.h` / `sfud/src/*.c`）无需改动
