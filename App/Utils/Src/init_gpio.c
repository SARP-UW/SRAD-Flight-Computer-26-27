/**
 * @file App/Utils/Inc/init_gpio.h
 * @authors Jude Merritt
 * @brief Initializes the GPIO pins for the flight computer. To
 *        make changes to gpio pin configurations, edit the gpio_configs array.
 */

#include "pinout.h"
#include "stm32f4xx_hal.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t mode;
    uint32_t pull;
    uint32_t speed;
    uint32_t alternate;
} gpio_config_t;

// This array defines the configuration for each GPIO pin used in the flight computer.
static const gpio_config_t gpio_configs[] = {
    // LEDs
    //PORT                  , PIN                   , MODE                , PULL       , SPEED             , ALTERNATE
    {MCU_LED_PORT           , MCU_LED_PIN           , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0},
    {APOGEE_PYRO_PORT       , APOGEE_PYRO_PIN       , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0},
    {APOGEE_BACKUP_PYRO_PORT, APOGEE_BACKUP_PYRO_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0},
    {MAIN_PYRO_PORT         , MAIN_PYRO_PIN         , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0},
    {MAIN_BACKUP_PYRO_PORT  , MAIN_BACKUP_PYRO_PIN  , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0},

    // MS5611 (Barometer)
    //PORT          , PIN          , MODE                , PULL       , SPEED                      , ALTERNATE
    {MS5611_CS_PORT  , MS5611_CS_PIN  , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW      , 0            },
    {MS5611_SCK_PORT , MS5611_SCK_PIN , GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},
    {MS5611_MISO_PORT, MS5611_MISO_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},
    {MS5611_MOSI_PORT, MS5611_MOSI_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},

    // BMI088 (Accelerometer)
    //PORT          , PIN          , MODE                , PULL       , SPEED                    , ALTERNATE
    {ACCEL_CS_PORT  , ACCEL_CS_PIN  , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW      , 0            },
    {ACCEL_SCK_PORT , ACCEL_SCK_PIN , GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},
    {ACCEL_MISO_PORT, ACCEL_MISO_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},
    {ACCEL_MOSI_PORT, ACCEL_MOSI_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},

    // BMI088 (Gyroscope)
    //PORT         , PIN          , MODE               , PULL       , SPEED                     , ALTERNATE
    {GYRO_CS_PORT  , GYRO_CS_PIN  , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW       , 0            },
    {GYRO_SCK_PORT , GYRO_SCK_PIN , GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH ,GPIO_AF5_SPI1 },
    {GYRO_MISO_PORT, GYRO_MISO_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH ,GPIO_AF5_SPI1 },
    {GYRO_MOSI_PORT, GYRO_MOSI_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH ,GPIO_AF5_SPI1 },

    // W25Q128JVPIQ (Flash Memory)
    //PORT                 , PIN                  , MODE               , PULL       , SPEED                    , ALTERNATE
    {EXTERN_FLASH_CS_PORT  , EXTERN_FLASH_CS_PIN  , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW      , 0            },
    {EXTERN_FLASH_SCK_PORT , EXTERN_FLASH_SCK_PIN , GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},
    {EXTERN_FLASH_MISO_PORT, EXTERN_FLASH_MISO_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},
    {EXTERN_FLASH_MOSI_PORT, EXTERN_FLASH_MOSI_PIN, GPIO_MODE_AF_PP    , GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI1},

    // PS1240P02BT (Piezo Buzzer)
    //PORT      , PIN       , MODE               , PULL       , SPEED              , ALTERNATE
    {BUZZER_PORT, BUZZER_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}
};

void init_gpio(void) {
    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Initialize GPIO pins based on the configuration array
    for (size_t i = 0; i < sizeof(gpio_configs) / sizeof(gpio_configs[0]); ++i) {
        const gpio_config_t *config = &gpio_configs[i];
        GPIO_InitTypeDef gpio_init = {0};
        gpio_init.Pin = config->pin;
        gpio_init.Mode = config->mode;
        gpio_init.Pull = config->pull;
        gpio_init.Speed = config->speed;
        gpio_init.Alternate = config->alternate;
        HAL_GPIO_Init(config->port, &gpio_init);
    }
}