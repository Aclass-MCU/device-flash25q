#ifndef ADEV_FLASH25Q_H
#define ADEV_FLASH25Q_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aDrv_basic.h"
#include "aDrv_qspi.h"
#include "sfud.h"

#define ADEV_FLASH_IOCTL_QSPI_FAST_READ    0x01

typedef struct {
    aDrvQspiConfig_t drv_config;
    uint8_t          flashIndex;
} aDevFlash25qConfig_t;

typedef struct {
    aDrvQspiHandle_t qspi;
    sfud_flash      *sfud_dev;
    uint32_t         size;
    uint8_t          init_ok;
} aDevFlash25qHandle_t;

void         aDevFlash25qConfigStructInit(aDevFlash25qConfig_t *config);
void         aDevFlash25qHandleStructInit(aDevFlash25qHandle_t *handle);
aDrvStatus_t aDevFlash25qInit(const aDevFlash25qConfig_t *config, aDevFlash25qHandle_t *handle);
aDrvStatus_t aDevFlash25qDeInit(aDevFlash25qHandle_t *handle);

aDrvStatus_t aDevFlash25qRead(aDevFlash25qHandle_t *handle, uint32_t addr, uint8_t *data, uint32_t size);
aDrvStatus_t aDevFlash25qWrite(aDevFlash25qHandle_t *handle, uint32_t addr, const uint8_t *data, uint32_t size);
aDrvStatus_t aDevFlash25qErase(aDevFlash25qHandle_t *handle, uint32_t addr, uint32_t size);
aDrvStatus_t aDevFlash25qChipErase(aDevFlash25qHandle_t *handle);

uint32_t     aDevFlash25qGetSize(aDevFlash25qHandle_t *handle);
aDrvStatus_t aDevFlash25qHandleIsValid(aDevFlash25qHandle_t *handle);
aDrvStatus_t aDevFlash25qIoCtl(aDevFlash25qHandle_t *handle, uint32_t cmd, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* ADEV_FLASH25Q_H */
