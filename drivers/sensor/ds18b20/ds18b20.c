/* Maxim DS18B20 Programmable Resolution 1-Wire Digital Thermometer
 *
 * Copyright (c) 2019 Sven Schwermer
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <sensor.h>
#include <w1.h>
#include <misc/byteorder.h>
#include <logging/log.h>
#include <string.h>

#define LOG_LEVEL CONFIG_SENSOR_LOG_LEVEL
LOG_MODULE_REGISTER(DS18B20);

struct ds18b20_device_data {
	struct device *bus_master;
	u8_t mem[9];
	int valid;
};

struct ds18b20_device_config {
	const char *bus_name;
};

static int ds18b20_sample_fetch(struct device *dev, enum sensor_channel chan)
{
	int max_trying = 10;
	struct ds18b20_device_data *ds18b20 = dev->driver_data;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_AMBIENT_TEMP) {
		return -ENOTSUP;
	}

	ds18b20->valid = 0;
	memset(ds18b20->mem, 0, sizeof(ds18b20->mem));

	while (max_trying--) {
		int delay;
		u8_t external_power;
		u8_t crc;

		if (w1_reset_select_slave(ds18b20->bus_master))
			continue;

		w1_write_8(ds18b20->bus_master, W1_READ_PSUPPLY);
		external_power = w1_read_8(ds18b20->bus_master);

		if (w1_reset_select_slave(ds18b20->bus_master))
			continue;

		delay = K_MSEC(750);
		if (!external_power)
			w1_next_pullup(ds18b20->bus_master, delay);

		w1_write_8(ds18b20->bus_master, W1_CONVERT_TEMP);

		if (external_power)
			k_sleep(delay);

		if (w1_reset_select_slave(ds18b20->bus_master))
			continue;

		w1_write_8(ds18b20->bus_master, W1_READ_SCRATCHPAD);
		w1_read_block(ds18b20->bus_master, ds18b20->mem, 9);

		crc = w1_calc_crc8(ds18b20->mem, 8);

		if (ds18b20->mem[8] == crc) {
			ds18b20->valid = 1;
			break;
		}
	}

	return (ds18b20->valid == 1) ? 0 : -EIO;
}

static int ds18b20_channel_get(struct device *dev, enum sensor_channel chan,
			       struct sensor_value *val)
{
	struct ds18b20_device_data *ds18b20 = dev->driver_data;

	if (chan != SENSOR_CHAN_AMBIENT_TEMP) {
		return -ENOTSUP;
	}

	s32_t t = sys_get_le16(ds18b20->mem);
	t *= (1000000 / 0x10);
	val->val1 = t / 1000000;
	val->val2 = t % 1000000;

	return 0;
}

static const struct sensor_driver_api ds18b20_api = {
	.sample_fetch = ds18b20_sample_fetch,
	.channel_get = ds18b20_channel_get,
};

static int ds18b20_init(struct device *dev)
{
	struct ds18b20_device_data *ds18b20 = dev->driver_data;
	const struct ds18b20_device_config *config = dev->config->config_info;

	ds18b20->bus_master = device_get_binding(config->bus_name);
	if (!ds18b20->bus_master) {
		LOG_DBG("1-Wire bus master controller not found: %s.",
			config->bus_name);
		return -EINVAL;
	}
	ds18b20->valid = 0;

	return 0;
}

static struct ds18b20_device_data ds18b20_data_1;

static struct ds18b20_device_config ds18b20_config_1 = {
	.bus_name = CONFIG_DS18B20_W1_INST,
};

DEVICE_AND_API_INIT(ds18b20_1, CONFIG_DS18B20_NAME, ds18b20_init,
		    &ds18b20_data_1, &ds18b20_config_1, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &ds18b20_api);
