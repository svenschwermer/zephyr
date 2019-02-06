#include <w1.h>

static const u8_t w1_crc8_table[] = {
	0,   94,  188, 226, 97,  63,  221, 131, 194, 156, 126, 32,  163, 253,
	31,  65,  157, 195, 33,  127, 252, 162, 64,  30,  95,  1,   227, 189,
	62,  96,  130, 220, 35,  125, 159, 193, 66,  28,  254, 160, 225, 191,
	93,  3,   128, 222, 60,  98,  190, 224, 2,   92,  223, 129, 99,  61,
	124, 34,  192, 158, 29,  67,  161, 255, 70,  24,  250, 164, 39,  121,
	155, 197, 132, 218, 56,  102, 229, 187, 89,  7,   219, 133, 103, 57,
	186, 228, 6,   88,  25,  71,  165, 251, 120, 38,  196, 154, 101, 59,
	217, 135, 4,   90,  184, 230, 167, 249, 27,  69,  198, 152, 122, 36,
	248, 166, 68,  26,  153, 199, 37,  123, 58,  100, 134, 216, 91,  5,
	231, 185, 140, 210, 48,  110, 237, 179, 81,  15,  78,  16,  242, 172,
	47,  113, 147, 205, 17,  79,  173, 243, 112, 46,  204, 146, 211, 141,
	111, 49,  178, 236, 14,  80,  175, 241, 19,  77,  206, 144, 114, 44,
	109, 51,  209, 143, 12,  82,  176, 238, 50,  108, 142, 208, 83,  13,
	239, 177, 240, 174, 76,  18,  145, 207, 45,  115, 202, 148, 118, 40,
	171, 245, 23,  73,  8,   86,  180, 234, 105, 55,  213, 139, 87,  9,
	235, 181, 54,  104, 138, 212, 149, 203, 41,  119, 244, 170, 72,  22,
	233, 183, 85,  11,  136, 214, 52,  106, 43,  117, 151, 201, 74,  20,
	246, 168, 116, 42,  200, 150, 21,  75,  169, 247, 182, 232, 10,  84,
	215, 137, 107, 53
};

static u8_t w1_read_bit(struct device *dev);
static void w1_write_bit(struct device *dev, int bit);
static void w1_pre_write(struct device *dev);
static void w1_post_write(struct device *dev);

u8_t w1_read_8(struct device *dev)
{
	u8_t res = 0;
	int i;
	for (i = 0; i < 8; ++i)
		res |= (w1_read_bit(dev) << i);
	return res;
}

void w1_write_8(struct device *dev, u8_t byte)
{
	int i;

	for (i = 0; i < 8; ++i) {
		if (i == 7)
			w1_pre_write(dev);
		w1_write_bit(dev, (byte >> i) & 0x1);
	}
	w1_post_write(dev);
}

int w1_reset_bus(struct device *dev)
{
	int result;
	const struct w1_driver_api *api = w1_api(dev);
	void *ctx = w1_data(dev)->context;

	int key = irq_lock();

	api->write_bit(ctx, 0);
	k_busy_wait(500);
	api->write_bit(ctx, 1);
	k_busy_wait(70);

	result = api->read_bit(ctx) & 0x1;

	irq_unlock(key);

	k_busy_wait(1000);

	return result;
}

// https://www.maximintegrated.com/en/app-notes/index.mvp/id/27
u8_t w1_calc_crc8(const u8_t *data, int len)
{
	u8_t crc = 0;
	while (len--)
		crc = w1_crc8_table[crc ^ *data++];
	return crc;
}

int w1_read_block(struct device *dev, u8_t *buf, int len)
{
	int i;
	for (i = 0; i < len; ++i)
		buf[i] = w1_read_8(dev);
	return len;
}

int w1_reset_select_slave(struct device *dev)
{
	int result;

	result = w1_reset_bus(dev);
	if (result)
		return result;

	// NOTE: Currently, we only support one slave on the bus
	w1_write_8(dev, W1_SKIP_ROM);

	return 0;
}

void w1_next_pullup(struct device *dev, int duration)
{
	w1_data(dev)->pullup_duration = duration;
}

static u8_t w1_read_bit(struct device *dev)
{
	int result;
	const struct w1_driver_api *api = w1_api(dev);
	void *ctx = w1_data(dev)->context;

	int key = irq_lock();

	api->write_bit(ctx, 0);
	k_busy_wait(6);
	api->write_bit(ctx, 1);
	k_busy_wait(9);

	result = api->read_bit(ctx);

	irq_unlock(key);

	k_busy_wait(55);

	return result & 0x1;
}

static void w1_write_bit(struct device *dev, int bit)
{
	const struct w1_driver_api *api = w1_api(dev);
	void *ctx = w1_data(dev)->context;

	int key = irq_lock();

	api->write_bit(ctx, 0);
	if (bit) {
		k_busy_wait(6);
		api->write_bit(ctx, 1);
		k_busy_wait(64);
	} else {
		k_busy_wait(60);
		api->write_bit(ctx, 1);
		k_busy_wait(10);
	}

	irq_unlock(key);
}

static void w1_pre_write(struct device *dev)
{
	struct w1_driver_data *data = w1_data(dev);
	const struct w1_driver_api *api = w1_api(dev);

	if (data->pullup_duration && api->set_pullup) {
		api->set_pullup(data->context, data->pullup_duration);
	}
}

static void w1_post_write(struct device *dev)
{
	struct w1_driver_data *data = w1_data(dev);
	const struct w1_driver_api *api = w1_api(dev);

	if (data->pullup_duration) {
		if (api->set_pullup)
			api->set_pullup(data->context, 0);
		else
			k_sleep(data->pullup_duration);
		data->pullup_duration = 0;
	}
}
