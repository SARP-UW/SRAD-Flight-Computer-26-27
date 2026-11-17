

/**
 * @file App/Devices/Src/w25q128jv.c
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
#include <string.h>

/** 
* Notes
* ! REMINDER In cubemx, set CS pin to be high on startup
* - Functions are designed to be non-blocking, with the exception of init and send_command
*     - Calling functions should check if the flash is busy before any operations
 */

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
#define PAGE_PROGRAM 0x02
#define CHIP_ERASE 0xC7
#define SECTOR_ERASE 0x20

#define READ_DATA 0x03

// SPI
extern SPI_HandleTypeDef hspi2;

/**************************************************************************************************
 * @section Private function definitions
 **************************************************************************************************/

// Returns a timeout value in ms based on how many bytes we expect to transfer, with extra margin
// If a transfer takes longer than this timeout, HAL assumes the transfer failed
static uint32_t spi_timeout_ms(uint32_t num_bytes) {
    // Calculates SPI frequency in Hz
    uint32_t br = (hspi2.Init.BaudRatePrescaler & SPI_CR1_BR) >> SPI_CR1_BR_Pos;
    uint32_t spi_hz = HAL_RCC_GetPCLK1Freq() >> (br + 1U);
    return 2 * (num_bytes * 8U * 1000U) / spi_hz + 5U; // 2x margin + 5ms extra margin
}

// Used to send a single command
static HAL_StatusTypeDef send_command(
    uint8_t command,
    uint8_t *tx_data,
    uint16_t tx_size,
    uint8_t *rx_data,
    uint32_t rx_size)
{
    HAL_StatusTypeDef status;

    // Flag empty pointers
    if ((tx_size > 0 && tx_data == NULL) || (rx_size > 0 && rx_data == NULL)) {
        return HAL_ERROR;
    }

    // Select flash chip
    HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_RESET);

    // Transmit instruction
    status = HAL_SPI_Transmit(&hspi2, &command, 1, spi_timeout_ms(1));
    if (status != HAL_OK) {
        HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_SET);
        return status;
    }

    // Transmit optional address and/or write data
    if (status == HAL_OK && tx_size > 0) {
        status = HAL_SPI_Transmit(&hspi2, tx_data, tx_size, spi_timeout_ms(tx_size));
    }
    if (status != HAL_OK) {
        HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_SET);
        return status;
    }

    // Receive response, if applicable
    // ! HAL_SPI_Receive is limited to a Size of type uint16_t. For simplicity, we split reads into chunks of 4096 bytes
    uint16_t chunk;
    while (status == HAL_OK && rx_size > 0) {
        if (rx_size >= 4096U) {
            chunk = 4096U;
        } else {
            chunk = (uint16_t)rx_size;
        }
        status = HAL_SPI_Receive(&hspi2, rx_data, chunk, spi_timeout_ms(chunk));
        
        rx_data += chunk;
        rx_size -= chunk; // No need to worry about underflow since chunk <= rx_size
    }

    HAL_GPIO_WritePin(EXTERN_FLASH_CS_PORT, EXTERN_FLASH_CS_PIN, GPIO_PIN_SET);

    return status;
}

static HAL_StatusTypeDef check_write_enable(void) {
    // Check if write enable was successful
    uint8_t sr1 = 0;
    HAL_StatusTypeDef status = send_command(
        SR1_READ, // Read Status Register 1 command
        NULL, 0,
        &sr1, 1
    );
    if (status != HAL_OK) {
        return status;
    }
    // Write enable bit is in position S1 of status register 1, make sure it is 1
    if ((sr1 & 0b00000010) == 0) { // Throw error if write enable bit is not set
        return HAL_ERROR;
    }
    return HAL_OK;
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

HAL_StatusTypeDef extern_flash_is_busy(void) {
    uint8_t status_reg = 0;
    bool busy;

    HAL_StatusTypeDef status = send_command(
        SR1_READ, // Read Status Register 1 command
        NULL, 0,
        &status_reg, 1
    );

    if (status != HAL_OK) {
        return status;
    }
    busy = (status_reg & 0b00000001) != 0; // Busy bit is the LSB of status register 1

    if (busy) {
        return HAL_BUSY;
    }
    return HAL_OK;
}

HAL_StatusTypeDef extern_flash_wait_until_ready(uint32_t max_timeout){
    uint32_t start_time = HAL_GetTick();
    HAL_StatusTypeDef status = extern_flash_is_busy();

    while (status != HAL_OK) {
        status = extern_flash_is_busy();
        if (status != HAL_OK && status != HAL_BUSY) {
            return status;
        }
        
        if ((HAL_GetTick() - start_time) >= max_timeout) {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

HAL_StatusTypeDef init_w25q128jv(void) {
    HAL_Delay(10); // Wait for flash to accept instructions
    HAL_StatusTypeDef status;

    // * Release from power-down state
    status = send_command(
        RELEASE_POWER_DOWN, // Release from power-down state
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }
    HAL_Delay(1);

    // * Reset the device
    status = send_command(
        ENABLE_RESET, // Enable resetting
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }

    status = send_command(
        RESET, // Perform reset
        NULL, 0,
        NULL, 0
    );
    HAL_Delay(1);
    if (status != HAL_OK) {
        return status;
    }

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
HAL_StatusTypeDef chip_erase(void) {
    HAL_StatusTypeDef status;
    status = extern_flash_is_busy();
    if (status != HAL_OK) {
        return status;
    }

    // * Enable writing to the flash
    status = send_command(
        WRITE_ENABLE, // Write Enable command
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }
    // Check if write enable was successful
    status = check_write_enable();
    if (status != HAL_OK) {
        return status;
    }

    // * Erase the entire chip
    status = send_command(
        CHIP_ERASE, // Chip Erase command
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }

    // Wait for the erase operation to complete
    // status = extern_flash_wait_until_ready(200000); // Wait up to 200 seconds for erase to complete, ref datasheet 9.6
    // if (status != HAL_OK)
    //     return status;

    return HAL_OK;
}

HAL_StatusTypeDef sector_erase(uint32_t address) {
    HAL_StatusTypeDef status;
    status = extern_flash_is_busy();
    if (status != HAL_OK) {
        return status;
    }

    // Ensure sector location is within the size of memory
    if (address >= FLASH_SIZE_BYTES || address + 4096U > FLASH_SIZE_BYTES) {
        return HAL_ERROR;
    }


    // Ensure sector address is aligned to 4096 bytes (sector size)
    if (address % 4096 != 0) {
        return HAL_ERROR;
    }

    // Enable writing to the flash
    status = send_command(
        WRITE_ENABLE, // Write Enable command
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }
    // Check if write enable was successful
    status = check_write_enable();
    if (status != HAL_OK) {
        return status;
    }

    // Erase the specified sector
    uint8_t address_bytes[3] = {
        (uint8_t)(address >> 16), // MSB
        (uint8_t)(address >> 8),
        (uint8_t)(0x00)          // Addresses are always aligned to 4096 bytes, so last byte is always 0
    };
    status = send_command(
        SECTOR_ERASE, // Sector Erase command
        address_bytes, 3,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }

    // Wait for the erase operation to complete
    // status = extern_flash_wait_until_ready(400); // Max sector erase duration, ref datasheet 9.6
    // if (status != HAL_OK)
    //     return status;

    return HAL_OK;
}

// Reads for specified length starting from input address, len is number of bytes to read
HAL_StatusTypeDef read_data(uint32_t address, uint8_t *buffer, uint32_t len) {
    HAL_StatusTypeDef status;
    // Ensure read is within the size of memory
    if (buffer == NULL || len == 0) {
        return HAL_ERROR;
    }
    if (address >= FLASH_SIZE_BYTES || len > FLASH_SIZE_BYTES - address) {
        return HAL_ERROR;
    }

    status = extern_flash_is_busy();
    if (status != HAL_OK) {
        return status;
    }

    // Convert 24-bit address to be sent in three separate 8-byte chunks, casting leaves the last byte after the bit shift
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

HAL_StatusTypeDef write_data(uint32_t address, uint8_t *data, uint16_t len) {
    HAL_StatusTypeDef status;
    if (data == NULL) {
        return HAL_ERROR;
    }
    // Ensure the write fits within flash memory
    if (address >= FLASH_SIZE_BYTES || address + len > FLASH_SIZE_BYTES) {
        return HAL_ERROR;
    }
    // Ensure the write fits within a single page (256 bytes)
    // This is assuming that we will never write across page boundaries
    if (address % 256 + len > 256) {
        return HAL_ERROR;
    }
    
    status = extern_flash_is_busy();
    if (status != HAL_OK) {
        return status;
    }

    // Write enable
    status = send_command(
        WRITE_ENABLE,
        NULL, 0,
        NULL, 0
    );
    if (status != HAL_OK) {
        return status;
    }

    // Check if write enable was successful
    status = check_write_enable();
    if (status != HAL_OK) {
        return status;
    }

    // Combine write command, address, and data
    uint8_t tx_buffer[3 + len]; // 3 bytes for address + len bytes for data
    tx_buffer[0] = (uint8_t)(address >> 16); // MSB
    tx_buffer[1] = (uint8_t)(address >> 8);
    tx_buffer[2] = (uint8_t)(address); // LSB
    memcpy(&tx_buffer[3], data, len);

    return send_command(
        PAGE_PROGRAM, // Write Data command
        tx_buffer, 3 + len,
        NULL, 0
    );
}