#include "aDev_flash25q.h"

extern void aDevFlash25qPortSetQspiHandle(aDrvQspiHandle_t *handle);

int __io_putchar(int ch)
{
    USART_TypeDef *usart = USART6;
    while (!(usart->ISR & USART_ISR_TXE_TXFNF));
    usart->TDR = (uint8_t)ch;
    return ch;
}

void aDevFlash25qConfigStructInit(aDevFlash25qConfig_t *config)
{
    if (config == NULL) return;
    aDrvQspiConfigStructInit(&config->drv_config);
    config->flashIndex = 0;
}

void aDevFlash25qHandleStructInit(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL) return;
    aDrvQspiHandleStructInit(&handle->qspi);
    handle->sfud_dev = NULL;
    handle->size = 0;
    handle->init_ok = 0;
}

aDrvStatus_t aDevFlash25qInit(const aDevFlash25qConfig_t *config, aDevFlash25qHandle_t *handle)
{
    if (config == NULL || handle == NULL)
        return ADRV_INVALID_PARAM;

    aDevFlash25qHandleStructInit(handle);

    if (aDrvQspiInitStatic(&config->drv_config, &handle->qspi) != ADRV_OK)
        return ADRV_UNDERFLOW;

    aDevFlash25qPortSetQspiHandle(&handle->qspi);

    if (sfud_init() != SFUD_SUCCESS)
    {
        aDrvQspiDeInitStatic(&handle->qspi);
        return ADRV_TIMEOUT;
    }

    handle->sfud_dev = sfud_get_device(config->flashIndex);
    if (handle->sfud_dev == NULL)
    {
        aDrvQspiDeInitStatic(&handle->qspi);
        return ADRV_NOT_INITIALIZED;
    }

    if (sfud_device_init(handle->sfud_dev) != SFUD_SUCCESS)
    {
        aDrvQspiDeInitStatic(&handle->qspi);
        return ADRV_NOT_SUPPORTED;
    }

    handle->size = handle->sfud_dev->chip.capacity;
    handle->init_ok = 1;

    return ADRV_OK;
}

aDrvStatus_t aDevFlash25qDeInit(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL)
        return ADRV_INVALID_PARAM;

    aDrvQspiDeInitStatic(&handle->qspi);

    handle->sfud_dev = NULL;
    handle->size = 0;
    handle->init_ok = 0;

    return ADRV_OK;
}

aDrvStatus_t aDevFlash25qRead(aDevFlash25qHandle_t *handle, uint32_t addr, uint8_t *data, uint32_t size)
{
    if (handle == NULL || handle->init_ok == 0 || data == NULL || size == 0)
        return ADRV_INVALID_PARAM;

    return (sfud_read(handle->sfud_dev, addr, size, data) == SFUD_SUCCESS)
           ? ADRV_OK : ADRV_ERROR;
}

aDrvStatus_t aDevFlash25qWrite(aDevFlash25qHandle_t *handle, uint32_t addr, const uint8_t *data, uint32_t size)
{
    if (handle == NULL || handle->init_ok == 0 || data == NULL || size == 0)
        return ADRV_INVALID_PARAM;

    return (sfud_write(handle->sfud_dev, addr, size, data) == SFUD_SUCCESS)
           ? ADRV_OK : ADRV_ERROR;
}

aDrvStatus_t aDevFlash25qErase(aDevFlash25qHandle_t *handle, uint32_t addr, uint32_t size)
{
    if (handle == NULL || handle->init_ok == 0)
        return ADRV_INVALID_PARAM;

    return (sfud_erase(handle->sfud_dev, addr, size) == SFUD_SUCCESS)
           ? ADRV_OK : ADRV_ERROR;
}

aDrvStatus_t aDevFlash25qChipErase(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL || handle->init_ok == 0)
        return ADRV_INVALID_PARAM;

    return (sfud_chip_erase(handle->sfud_dev) == SFUD_SUCCESS)
           ? ADRV_OK : ADRV_ERROR;
}

uint32_t aDevFlash25qGetSize(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL) return 0;
    return handle->size;
}

aDrvStatus_t aDevFlash25qHandleIsValid(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL || handle->init_ok == 0 || handle->sfud_dev == NULL)
        return ADRV_INVALID_PARAM;
    return ADRV_OK;
}

aDrvStatus_t aDevFlash25qIoCtl(aDevFlash25qHandle_t *handle, uint32_t cmd, void *arg)
{
    if (handle == NULL || handle->init_ok == 0)
        return ADRV_INVALID_PARAM;

    switch (cmd)
    {
    case ADEV_FLASH_IOCTL_QSPI_FAST_READ:
        return (sfud_qspi_fast_read_enable(handle->sfud_dev, (uint8_t)(uint32_t)arg) == SFUD_SUCCESS)
               ? ADRV_OK : ADRV_ERROR;
    default:
        return ADRV_INVALID_PARAM;
    }
}
