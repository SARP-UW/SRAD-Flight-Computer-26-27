

/**
 * @file App/Devices/Src/extern_flash.c
 * @authors Michael Zheng
 * @brief W25Q128JV external flash driver
 * 
 * Datasheet: W25Q128JVPIQ — JLCPCB C190862
 * https://www.winbond.com/resource-files/w25q128jv%20revf%2003272018%20plus.pdf
 */

#include "w25q128jv.h"
#include "spi.h"
#include "gpio.h"
#include "pinout.h"

/** 
* Notes
* ! REMINDER In cubemx, set CS pin to be high on startup
 */

/**************************************************************************************************
 * @section Type definitions
 **************************************************************************************************/

// ref: 
typedef struct {
    
}; 

/**************************************************************************************************
 * @section Definitions and global variables
 **************************************************************************************************/

#define FLASH_SIZE_BYTES (16UL * 1024 * 1024) // 16,777,216 bytes = 16MB
// Basic Commands
// ref: Datasheet page 20
#define SR1_READ    0x05
#define RELEASE_POWER_DOWN 0xAB
#define ENABLE_RESET 0x66
#define RESET       0x99
#define JEDEC_ID_READ 0x9F

#define WRITE_ENABLE 0x06
#define CHIP_ERASE 0xC7
#define SECTOR_ERASE 0x20

#define READ_DATA 0x03

// SPI
extern SPI_HandleTypeDef hspi2;
static const uint32_t timeout = 20; // 20ms timeout for commands

/**************************************************************************************************
 * @section Private function definitions
 **************************************************************************************************/

// Used to send a single command
static HAL_StatusTypeDef send_command(
    uint8_t command,
    const uint8_t *tx_data,
    uint16_t tx_size,
    uint8_t *rx_data,
    uint16_t rx_size)
{
    HAL_StatusTypeDef status;

    // Select flash chip
    HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_RESET);

    // Transmit instruction
    status = HAL_SPI_Transmit(&hspi2, &command, 1, timeout);

    // Transmit optional address and/or write data
    if (status == HAL_OK && tx_size > 0) {
        status = HAL_SPI_Transmit(&hspi2, tx_data, tx_size, timeout);
    }

    // Receive response, if applicable
    if (status == HAL_OK && rx_size > 0) {
    	status = HAL_SPI_Receive(&hspi2, rx_data, rx_size, timeout);
	}

    HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_SET);

    return status;
}

HAL_StatusTypeDef is_busy(bool *busy) {
    uint8_t status_reg = 0;

    if (busy == NULL) return HAL_ERROR;

    HAL_StatusTypeDef status = send_command(
        SR1_READ, // Read Status Register 1 command
        NULL, 0,
        &status_reg, 1
    );

    if (status != HAL_OK)
        return status;
    *busy = (status_reg & 0b00000001) != 0; // Busy bit is the LSB of status register 1
    return HAL_OK;
}

HAL_StatusTypeDef wait_until_ready(uint32_t max_timeout){
    uint32_t start_time = HAL_GetTick();
    bool busy = true;

    while (busy){
        HAL_StatusTypeDef status = is_busy(&busy);

        if (status != HAL_OK)
            return status;

        if (!busy)
            return HAL_OK;

        if ((HAL_GetTick() - start_time) >= max_timeout)
            return HAL_TIMEOUT;
    }

    return HAL_OK;
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

HAL_StatusTypeDef init_w25q128jv(void) {
	HAL_StatusTypeDef status;

	// * Release from power-down state
	status = send_command(
        RELEASE_POWER_DOWN, // Release from power-down state
        NULL, 0,
        NULL, 0
	);
	if (status != HAL_OK)
        return status;
	HAL_Delay(1);

	// * Reset the device
	status = send_command(
		ENABLE_RESET, // Enable resetting
		NULL, 0,
		NULL, 0
	);
	if (status != HAL_OK)
        return status;

	status = send_command(
		RESET, // Perform reset
		NULL, 0,
		NULL, 0
	);
	HAL_Delay(1);
	if (status != HAL_OK)
        return status;

    // * Verify JEDEC ID
    // ref: Page 19 of the datasheet
	uint8_t jedec_id[3];
	status = send_command(
		JEDEC_ID_READ, // Read JEDEC ID command
		NULL, 0,
		jedec_id, 3
	);
    if (status != HAL_OK) {
        return status;
    } else if (jedec_id[0] != 0xEF || jedec_id[1] != 0x40 || jedec_id[2] != 0x18) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

// Fully erase the entire flash chip. This operation takes max 200sec to complete.
HAL_StatusTypeDef erase_w15q128jv(void) {
    HAL_StatusTypeDef status;

    // * Enable writing to the flash
    status = send_command(
        WRITE_ENABLE, // Write Enable command
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK)
        return status;

    // * Erase the entire chip
    status = send_command(
        CHIP_ERASE, // Chip Erase command
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK)
        return status;

    // * Wait for the erase operation to complete
    status = wait_until_ready(200000); // Wait up to 200 seconds for erase to complete, ref datasheet 9.6
    if (status != HAL_OK)
        return status;

    return HAL_OK;
}

// Reads for specified length starting from input address, len is number of bytes to read.
static HAL_StatusTypeDef read_data(uint32_t address, uint8_t *buffer, uint32_t len) {
    // Ensure read is within the size of memory
    if (address >= FLASH_SIZE_BYTES || address + len > FLASH_SIZE_BYTES) {
        return HAL_ERROR;
    }

    // Wait until flash is ready
    HAL_StatusTypeDef status = wait_until_ready(100);
    if (status != HAL_OK)
        return status;

    // Convert 24-bit address to be sent in three separate 8-byte chunks
    uint8_t address_bytes[3] = {
        (uint8_t)(address >> 16), // MSB
        (uint8_t)(address >> 8),
        (uint8_t)(address) // LSB
    };

    return send_command(
        READ_DATA, // Read Data command
        address_bytes, 3,
        buffer, len
    );
}

// Note that the page number is zero-indexed.
HAL_StatusTypeDef read_page(uint32_t page, uint8_t *buffer){
    return read_data(
        page * 256, // page * 256 bytes/page
        buffer,
        256
    );
}

// ! wip
static HAL_StatusTypeDef write_data(uint32_t address, const uint8_t *data, uint32_t len) {
    if (data == NULL) 
        return HAL_ERROR;
    // Ensure the write fits within flash memory
    if (address >= FLASH_SIZE_BYTES || address + len > FLASH_SIZE_BYTES) {
        return HAL_ERROR;
    }

    // Write enable
    HAL_StatusTypeDef status = send_command(
        WRITE_ENABLE,
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK)
        return status;

    // Check if write enable was successful
    uint8_t status_reg = 0;
    HAL_StatusTypeDef status = send_command(
        SR1_READ, // Read Status Register 1 command
        NULL, 0,
        &status_reg, 1
    );
    if (status != HAL_OK)
        return status;
    // Busy bit is in position S1 of status register 1
    if ((status_reg & 0b00000010) == 0)
        return HAL_ERROR;



    // Datsheet page 34:
    // "If an entire 256 byte page is to be programmed, the last address byte (the 8 least significant address bits) should be set to 0"
    return status;
}