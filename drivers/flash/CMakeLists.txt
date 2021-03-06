zephyr_library()

zephyr_library_sources_ifdef(CONFIG_SPI_NOR spi_nor.c)
zephyr_library_sources_ifdef(CONFIG_SPI_FLASH_W25QXXDV spi_flash_w25qxxdv.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_QMSI soc_flash_qmsi.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_NRF soc_flash_nrf.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_MCUX soc_flash_mcux.c)
zephyr_library_sources_ifdef(CONFIG_FLASH_PAGE_LAYOUT flash_page_layout.c)
zephyr_library_sources_ifdef(CONFIG_USERSPACE flash_handlers.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_SAM0 flash_sam0.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_SAM flash_sam.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_NIOS2_QSPI soc_flash_nios2_qspi.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_GECKO flash_gecko.c)

if(CONFIG_SOC_SERIES_STM32F0X)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_STM32
	flash_stm32.c
	flash_stm32f0x.c
	)
endif()

if(CONFIG_SOC_SERIES_STM32F3X)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_STM32 flash_stm32f3x.c)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_STM32 flash_stm32f3x_priv.c)
endif()

if(CONFIG_SOC_SERIES_STM32F4X)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_STM32
	flash_stm32.c
	flash_stm32f4x.c
	)
endif()

if(CONFIG_SOC_SERIES_STM32F7X)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_STM32
	flash_stm32.c
	flash_stm32f7x.c
	)
endif()

if(CONFIG_SOC_SERIES_STM32L4X)
zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_STM32
	flash_stm32.c
	flash_stm32l4x.c
	)
endif()

zephyr_include_directories_ifdef(
	CONFIG_SOC_FLASH_NRF_RADIO_SYNC
	${ZEPHYR_BASE}/subsys/bluetooth
	${ZEPHYR_BASE}/subsys/bluetooth/controller/ll_sw/nordic
	)

zephyr_library_sources_ifdef(CONFIG_FLASH_SHELL flash_shell.c)
