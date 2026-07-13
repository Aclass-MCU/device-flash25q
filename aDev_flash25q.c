/**
 * @file aDev_flash25q.c
 * @brief aDev 25Q系列SPI Flash设备驱动实现（基于SFUD封装）
 * @version 1.0
 * @date 2025
 * @author AClass Development Team
 *
 * @par 功能描述:
 * 基于SFUD库封装的25Q系列SPI Flash设备驱动，提供简化的Flash操作接口
 * 不修改SFUD原始代码，仅做简单封装
 */

// ==================== 包含文件 ====================
#include "aDev_flash25q.h"
#include "sfud.h"

// ==================== 公共接口函数实现 ====================

aDevFlashStatus_t aDevFlash25qInit(aDevFlash25qHandle_t *handle, size_t index)
{
    if (handle == NULL)
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    // 初始化SFUD库
    if (sfud_init() != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    // 获取SFUD设备
    handle->sfud_dev = sfud_get_device(index);
    if (handle->sfud_dev == NULL)
    {
        return ADEV_FLASH_NOT_FOUND;
    }

    // 初始化SFUD设备
    if (sfud_device_init(handle->sfud_dev) != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    // 获取Flash容量
    handle->size = handle->sfud_dev->chip.capacity;
    handle->init_ok = 1;

    return ADEV_FLASH_OK;
}

aDevFlashStatus_t aDevFlash25qDeInit(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL)
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    handle->sfud_dev = NULL;
    handle->size = 0;
    handle->init_ok = 0;

    return ADEV_FLASH_OK;
}

aDevFlashStatus_t aDevFlash25qRead(aDevFlash25qHandle_t *handle, uint32_t addr, uint32_t size, uint8_t *data)
{
    if ((handle == NULL) || (data == NULL) || (size == 0))
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    if ((handle->init_ok == 0) || (handle->sfud_dev == NULL))
    {
        return ADEV_FLASH_ERROR;
    }

    if (sfud_read(handle->sfud_dev, addr, size, data) != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    return ADEV_FLASH_OK;
}

aDevFlashStatus_t aDevFlash25qWrite(aDevFlash25qHandle_t *handle, uint32_t addr, const uint8_t *data, uint32_t size)
{
    if ((handle == NULL) || (data == NULL) || (size == 0))
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    if ((handle->init_ok == 0) || (handle->sfud_dev == NULL))
    {
        return ADEV_FLASH_ERROR;
    }

    if (sfud_write(handle->sfud_dev, addr, size, data) != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    return ADEV_FLASH_OK;
}

aDevFlashStatus_t aDevFlash25qErase(aDevFlash25qHandle_t *handle, uint32_t addr, uint32_t size)
{
    if (handle == NULL)
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    if ((handle->init_ok == 0) || (handle->sfud_dev == NULL))
    {
        return ADEV_FLASH_ERROR;
    }

    if (sfud_erase(handle->sfud_dev, addr, size) != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    return ADEV_FLASH_OK;
}

aDevFlashStatus_t aDevFlash25qEraseWrite(aDevFlash25qHandle_t *handle, uint32_t addr, const uint8_t *data, uint32_t size)
{
    if ((handle == NULL) || (data == NULL) || (size == 0))
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    if ((handle->init_ok == 0) || (handle->sfud_dev == NULL))
    {
        return ADEV_FLASH_ERROR;
    }

    if (sfud_erase_write(handle->sfud_dev, addr, size, data) != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    return ADEV_FLASH_OK;
}

aDevFlashStatus_t aDevFlash25qChipErase(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL)
    {
        return ADEV_FLASH_INVALID_PARAM;
    }

    if ((handle->init_ok == 0) || (handle->sfud_dev == NULL))
    {
        return ADEV_FLASH_ERROR;
    }

    if (sfud_chip_erase(handle->sfud_dev) != SFUD_SUCCESS)
    {
        return ADEV_FLASH_ERROR;
    }

    return ADEV_FLASH_OK;
}

uint32_t aDevFlash25qGetSize(aDevFlash25qHandle_t *handle)
{
    if (handle == NULL)
    {
        return 0;
    }

    return handle->size;
}
