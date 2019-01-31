/*
 * Copyright (c) 2019 Sven Schwermer
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sensor.h>

/* Driver config */
struct w1_gpio_config {
	char *gpio_name;
	u8_t data_pin;
};

/* Driver instance data */
struct w1_gpio_context {
	struct device *gpio;
	u8_t data_pin;
};

static struct sensor_driver_api api = {

};

static int w1_gpio_init(struct device *dev)
{
	struct w1_gpio_context *context = dev->driver_data;
	const struct w1_gpio_config *config = dev->config->config_info;

	context->gpio = device_get_binding(config->gpio_name);
	if (!context->gpio) {
		return -EINVAL;
	}
	context->data_pin = config->data_pin;

	dev->driver_api = &api;

	return 0;
}

static struct w1_gpio_context w1_gpio_dev_data_1;

static const struct w1_gpio_config w1_gpio_dev_cfg_1 = {
	.gpio_name = DT_ONEWIRE_1_GPIO_DEV_NAME,
	.data_pin = DT_ONEWIRE_1_GPIO_PIN_NUM,
};

DEVICE_INIT(w1_gpio_1, DT_ONEWIRE_1_NAME, w1_gpio_init, &w1_gpio_dev_data_1,
	    &w1_gpio_dev_cfg_1, PRE_KERNEL_2, CONFIG_W1_INIT_PRIORITY);
