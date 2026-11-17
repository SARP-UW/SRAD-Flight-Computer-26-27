/**
 * @file App/Devices/Inc/w25q128jv.h
 * @authors Michael Zheng
 * @brief W25Q128JV external flash driver
**/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/**************************************************************************************************
 * @section Function declarations
 **************************************************************************************************/

/**
 * @brief Wakes the flash from power-down, resets it, and verifies the JEDEC ID (EF 40 18).
 *        This is a blocking function that terminates all on-going operations and erases volatile settings.
 *
 * @return HAL_OK on success, HAL_ERROR if the JEDEC ID does not match, or a HAL SPI error code.
 */
HAL_StatusTypeDef init_w25q128jv(void);

/**
 * @brief Reads the BUSY bit of status register 1.
 *
 * @return HAL_OK if the flash is idle, HAL_BUSY if a write/erase is in progress,
 *         or a HAL SPI error code.
 */
HAL_StatusTypeDef extern_flash_is_busy(void);

/**
 * @brief Blocks until the flash is idle or the timeout expires.
 *
 * @param max_timeout Maximum time to wait in ms.
 * @return HAL_OK once idle, HAL_TIMEOUT if still busy after max_timeout, or a HAL SPI error code.
 */
HAL_StatusTypeDef extern_flash_wait_until_ready(uint32_t max_timeout);

/**
 * @brief Starts a full chip erase (non-blocking). Maximum completion time of 200ms (Datasheet 9.6).
 *
 * @return HAL_OK if the erase was started, HAL_BUSY if the flash is busy,
 *         HAL_ERROR if write enable failed, or a HAL SPI error code.
 */
HAL_StatusTypeDef chip_erase(void);

/**
 * @brief Starts erasing one 4 KB sector. Takes up to 400 ms to complete.
 *
 * @param address Start address of the sector, which must be a multiple of 4096.
 * @return HAL_OK if the erase was started, HAL_BUSY if the flash is busy,
 *         HAL_ERROR if the address is invalid or write enable failed, or a HAL SPI error code.
 */
HAL_StatusTypeDef sector_erase(uint32_t address);

/**
 * @brief Reads len bytes starting at address into a buffer.
 *
 * @param address Start address (0 to 0xFFFFFF).
 * @param buffer  Destination buffer, at least len bytes long.
 * @param len     Number of bytes to read.
 * @return HAL_OK on success, HAL_BUSY if the flash is busy,
 *         HAL_ERROR if the range is invalid, or a HAL SPI error code.
 */
HAL_StatusTypeDef read_data(uint32_t address, uint8_t *buffer, uint32_t len);

/**
 * @brief Starts programming len bytes at address. The target space in memory must be erased beforehand.
 * @param address Start address. The write cannot cross a page boundary.
 * @param data    Source data, len bytes long.
 * @param len     Number of bytes to write (1 to 256).
 * @return HAL_OK if programming was started, HAL_BUSY if the flash is busy,
 *         HAL_ERROR if the range is invalid or write enable failed, or a HAL SPI error code.
 */
HAL_StatusTypeDef write_data(uint32_t address, uint8_t *data, uint16_t len);
