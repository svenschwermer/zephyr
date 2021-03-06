/* This file is a temporary workaround for mapping of the generated information
 * to the current driver definitions.  This will be removed when the drivers
 * are modified to handle the generated information, or the mapping of
 * generated data matches the driver definitions.
 */

#define CONFIG_BT_SPI_DEV_NAME			DT_ST_STM32_SPI_FIFO_40003C00_ST_SPBTLE_RF_0_BUS_NAME
#define CONFIG_BT_SPI_MAX_CLK_FREQ		DT_ST_STM32_SPI_FIFO_40003C00_ST_SPBTLE_RF_0_SPI_MAX_FREQUENCY

#define CONFIG_BT_SPI_IRQ_DEV_NAME		BT_IRQ_GPIOS_CONTROLLER
#define CONFIG_BT_SPI_IRQ_PIN			BT_IRQ_GPIOS_PIN
#define CONFIG_BT_SPI_CHIP_SELECT_DEV_NAME	DT_ST_STM32_SPI_FIFO_40003C00_CS_GPIOS_CONTROLLER_0
#define CONFIG_BT_SPI_CHIP_SELECT_PIN		DT_ST_STM32_SPI_FIFO_40003C00_CS_GPIOS_PIN_0
#define CONFIG_BT_SPI_RESET_DEV_NAME		BT_RESET_GPIOS_CONTROLLER
#define CONFIG_BT_SPI_RESET_PIN			BT_RESET_GPIOS_PIN
