/**
 * @file aDev_flash25q.h
 * @brief aDev 25Q系列SPI Flash设备驱动接口（基于SFUD封装）
 * @version 1.0
 * @date 2025
 * @author AClass Development Team
 *
 * @par 功能描述:
 * 基于SFUD库封装的25Q系列SPI Flash设备驱动，提供简化的Flash操作接口
 */

#ifndef __ADEV_FLASH25Q_H
#define __ADEV_FLASH25Q_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sfud.h"

// ==================== Flash状态定义 ====================

/**
 * @brief Flash设备状态
 */
typedef enum {
    ADEV_FLASH_OK = 0,              ///< 操作成功
    ADEV_FLASH_ERROR = -1,          ///< 操作失败
    ADEV_FLASH_BUSY = -2,           ///< 设备忙
    ADEV_FLASH_TIMEOUT = -3,        ///< 超时
    ADEV_FLASH_INVALID_PARAM = -4,  ///< 参数无效
    ADEV_FLASH_NOT_FOUND = -5,      ///< 未找到Flash设备
} aDevFlashStatus_t;

// ==================== Flash设备结构体 ====================

/**
 * @brief Flash设备句柄
 */
typedef struct {
    sfud_flash *sfud_dev;           ///< SFUD设备指针
    uint32_t size;                  ///< Flash容量（字节）
    uint8_t init_ok;                ///< 初始化完成标志
} aDevFlash25qHandle_t;

// ==================== 公共接口函数 ====================

/**
 * @brief Flash设备初始化
 * @param handle Flash设备句柄指针
 * @param index SFUD设备索引号（默认为0）
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qInit(aDevFlash25qHandle_t *handle, size_t index);

/**
 * @brief Flash设备反初始化
 * @param handle Flash设备句柄指针
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qDeInit(aDevFlash25qHandle_t *handle);

/**
 * @brief 读取Flash数据
 * @param handle Flash设备句柄指针
 * @param addr 起始地址
 * @param size 读取大小
 * @param data 数据缓冲区指针
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qRead(aDevFlash25qHandle_t *handle, uint32_t addr, uint32_t size, uint8_t *data);

/**
 * @brief 写入Flash数据（不擦除）
 * @param handle Flash设备句柄指针
 * @param addr 起始地址
 * @param data 数据缓冲区指针
 * @param size 写入大小
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qWrite(aDevFlash25qHandle_t *handle, uint32_t addr, const uint8_t *data, uint32_t size);

/**
 * @brief 擦除Flash数据
 * @param handle Flash设备句柄指针
 * @param addr 起始地址
 * @param size 擦除大小
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qErase(aDevFlash25qHandle_t *handle, uint32_t addr, uint32_t size);

/**
 * @brief 擦除并写入Flash数据
 * @param handle Flash设备句柄指针
 * @param addr 起始地址
 * @param data 数据缓冲区指针
 * @param size 写入大小
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qEraseWrite(aDevFlash25qHandle_t *handle, uint32_t addr, const uint8_t *data, uint32_t size);

/**
 * @brief 全片擦除
 * @param handle Flash设备句柄指针
 * @return aDevFlashStatus_t 操作状态
 */
aDevFlashStatus_t aDevFlash25qChipErase(aDevFlash25qHandle_t *handle);

/**
 * @brief 获取Flash容量
 * @param handle Flash设备句柄指针
 * @return uint32_t Flash容量（字节）
 */
uint32_t aDevFlash25qGetSize(aDevFlash25qHandle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* __ADEV_FLASH25Q_H */
