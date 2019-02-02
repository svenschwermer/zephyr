/*
 * Copyright (c) 2019 Sven Schwermer
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public API for 1-Wire drivers and applications
 */

#ifndef ZEPHYR_INCLUDE_W1_H_
#define ZEPHYR_INCLUDE_W1_H_

/**
 * @brief 1-Wire Interface
 * @defgroup w1_interface 1-Wire Interface
 * @ingroup io_interfaces
 * @{
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define W1_SEARCH 0xF0
#define W1_ALARM_SEARCH 0xEC
#define W1_CONVERT_TEMP 0x44
#define W1_SKIP_ROM 0xCC
#define W1_COPY_SCRATCHPAD 0x48
#define W1_WRITE_SCRATCHPAD 0x4E
#define W1_READ_SCRATCHPAD 0xBE
#define W1_READ_ROM 0x33
#define W1_READ_PSUPPLY 0xB4
#define W1_MATCH_ROM 0x55
#define W1_RESUME_CMD 0xA5

/**
 * @brief 1-Wire driver API
 * This is the mandatory API any 1-Wire driver needs to expose.
 */
struct w1_driver_api {
	u8_t (*read_bit)(void *);
	void (*write_bit)(void *, u8_t);
	u8_t (*set_pullup)(void *, int);
};

struct w1_driver_data {
	void *context;
	int pullup_duration;
};

static inline const struct w1_driver_api *w1_api(struct device *dev)
{
	return (const struct w1_driver_api *)dev->driver_api;
}

static inline struct w1_driver_data *w1_data(struct device *dev)
{
	return (struct w1_driver_data *)dev->driver_data;
}

u8_t w1_read_8(struct device *dev);
void w1_write_8(struct device *dev, u8_t byte);
int w1_reset_bus(struct device *);
u8_t w1_calc_crc8(const u8_t *, int);
int w1_read_block(struct device *, u8_t *, int);
int w1_reset_select_slave(struct device *);
void w1_next_pullup(struct device *, int);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_W1_H_ */
