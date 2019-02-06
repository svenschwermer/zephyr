/*
 * Copyright (c) 2019 Sven Schwermer
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <w1.h>
#include <gpio.h>

/* Driver config */
struct w1_gpio_config {
	char *gpio_name;
	u32_t data_pin;
};

/* Driver instance data */
struct w1_gpio_context {
	struct device *gpio;
	u32_t data_pin;
};

#ifdef NRF_DRIVER
#include <nrf.h>
#endif

static u8_t read_bit(void *context)
{
	struct w1_gpio_context *ctx = context;
#ifdef NRF_DRIVER
	NRF_P0->DIRCLR = 1ul << ctx->data_pin;
	return (NRF_P0->IN >> ctx->data_pin) & 0x1;
#else
	u32_t value = 0;
	gpio_pin_configure(ctx->gpio, ctx->data_pin,
			   GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_DS_DFLT_LOW |
				   GPIO_DS_DISCONNECT_HIGH);
	gpio_pin_read(ctx->gpio, ctx->data_pin, &value);
	return value & 0x1;
#endif
}

static void write_bit(void *context, u8_t bit)
{
	struct w1_gpio_context *ctx = context;
#ifdef NRF_DRIVER
	NRF_P0->DIRSET = 1ul << ctx->data_pin;
	if (bit)
		NRF_P0->OUTSET = 1ul << ctx->data_pin;
	else
		NRF_P0->OUTCLR = 1ul << ctx->data_pin;
#else
	gpio_pin_configure(ctx->gpio, ctx->data_pin,
			   GPIO_DIR_OUT | GPIO_PUD_PULL_UP | GPIO_DS_DFLT_LOW |
				   GPIO_DS_DISCONNECT_HIGH);
	gpio_pin_write(ctx->gpio, ctx->data_pin, bit);
#endif
}

static struct w1_driver_api api = {
	.read_bit = read_bit,
	.write_bit = write_bit,
};

static int w1_gpio_init(struct device *dev)
{
	int result;
	struct w1_driver_data *data = dev->driver_data;
	struct w1_gpio_context *ctx = data->context;
	const struct w1_gpio_config *config = dev->config->config_info;

	ctx->gpio = device_get_binding(config->gpio_name);
	if (!ctx->gpio) {
		return -EINVAL;
	}
	ctx->data_pin = config->data_pin;

	result = gpio_pin_configure(ctx->gpio, ctx->data_pin,
				    GPIO_DIR_IN | GPIO_PUD_PULL_UP |
					    GPIO_DS_DFLT_LOW |
					    GPIO_DS_DISCONNECT_HIGH);
	if (result)
		return result;

	write_bit(ctx, 1);
	return 0;
}

static struct w1_gpio_context w1_gpio_dev_ctx_1;
static struct w1_driver_data w1_gpio_dev_data_1 = {
	.context = &w1_gpio_dev_ctx_1
};

static const struct w1_gpio_config w1_gpio_dev_cfg_1 = {
	.gpio_name = DT_ONEWIRE_1_GPIO_DEV_NAME,
	.data_pin = DT_ONEWIRE_1_GPIO_PIN_NUM,
};

DEVICE_AND_API_INIT(w1_gpio_1, DT_ONEWIRE_1_NAME, w1_gpio_init,
		    &w1_gpio_dev_data_1, &w1_gpio_dev_cfg_1, PRE_KERNEL_2,
		    CONFIG_W1_INIT_PRIORITY, &api);
