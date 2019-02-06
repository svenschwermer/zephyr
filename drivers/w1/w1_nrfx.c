/*
 * Copyright (c) 2019 Sven Schwermer
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <w1.h>
#include <hal/nrf_gpio.h>

#if defined(CONFIG_W1_NRFX_GPIO_0)
#define NRF_Px NRF_P0
#define PORT_NO 0
#elif defined(CONFIG_W1_NRFX_GPIO_1)
#define NRF_Px NRF_P1
#define PORT_NO 1
#else
#error "No nRF GPIO instance defined"
#endif

static u8_t read_bit(void *context)
{
	NRF_Px->DIRCLR = 1ul << CONFIG_W1_NRFX_GPIO_PIN;
	return (NRF_Px->IN >> CONFIG_W1_NRFX_GPIO_PIN) & 0x1;
}

static void write_bit(void *context, u8_t bit)
{
	NRF_Px->DIRSET = 1ul << CONFIG_W1_NRFX_GPIO_PIN;
	if (bit)
		NRF_Px->OUTSET = 1ul << CONFIG_W1_NRFX_GPIO_PIN;
	else
		NRF_Px->OUTCLR = 1ul << CONFIG_W1_NRFX_GPIO_PIN;
}

static struct w1_driver_api api = {
	.read_bit = read_bit,
	.write_bit = write_bit,
};

static int w1_nrfx_init(struct device *dev)
{
	nrf_gpio_cfg(NRF_GPIO_PIN_MAP(PORT_NO, CONFIG_W1_NRFX_GPIO_PIN),
		     NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT,
		     NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1,
		     NRF_GPIO_PIN_NOSENSE);
	write_bit(NULL, 1);
	return 0;
}

static struct w1_driver_data w1_nrfx_dev_data;

DEVICE_AND_API_INIT(w1_nrfx, CONFIG_W1_NRFX_NAME, w1_nrfx_init,
		    &w1_nrfx_dev_data, NULL, PRE_KERNEL_2,
		    CONFIG_W1_INIT_PRIORITY, &api);
