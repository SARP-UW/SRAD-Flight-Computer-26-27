

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
* - REMINDER In cubemx, set CS pin to be high by default
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

// Basic Commands
// ref: Page 10 of the datasheet


// SPI
extern SPI_HandleTypeDef hspi2;
static const uint32_t timeout = 10; // 10ms timeout for commands

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

    // Transmit optional address or write data
    if (status == HAL_OK && tx_size > 0) {
        status = HAL_SPI_Transmit(&hspi2, tx_data, tx_size, timeout);
    }

    // Receive response, if applicable
    if (status == HAL_OK && rx_size > 0) {
    	status = HAL_SPI_Receive(&hspi2, rx_data, rx_size, timeout);
	}

    // Deselect CS
    HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_SET);

    return status;
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

HAL_StatusTypeDef init_w25q128jv(void) {
	HAL_StatusTypeDef status;

	// * Release from power-down state
	status = send_command(
		0xAB,
		NULL,
		0,
		NULL, 
		0
	);
	if (status != HAL_OK) return status;
	HAL_Delay(1);

	// * Reset the device
	status = send_command(
		0x66, // Enable reset
		NULL,
		0,
		NULL,
		0
	);
	if (status != HAL_OK) return status;
	status = send_command(
		0x99, // Perform reset
		NULL,
		0,
		NULL,
		0
	);
	HAL_Delay(1);
	if (status != HAL_OK) return status;

    // * Verify JEDEC ID
    // ref: Page 19 of the datasheet
	uint8_t jedec_id[3];
	status = send_command(
		0x9F, // Read JEDEC ID command
		NULL, // No data to transmit
		0,
		jedec_id,
		3
	);
    if (status != HAL_OK) {
        return status;
    } else if (jedec_id[0] != 0xEF || jedec_id[1] != 0x40 || jedec_id[2] != 0x18) {
        return HAL_ERROR;
    }

    // ? (is this needed?) Verify no block protection is set 
    uint8_t sr1; // Status Register 1
    if ((status = flash_read_sr1(&sr1)) != HAL_OK) return status;         // 0x05
    if (sr1 & 0x1C) return HAL_ERROR;  

    return HAL_OK;
}


}