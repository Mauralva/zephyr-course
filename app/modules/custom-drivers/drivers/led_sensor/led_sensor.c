#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "custom_drivers/led_sensor.h"

#define DT_DRV_COMPAT custom_led_sensor

LOG_MODULE_REGISTER(led_sensor, CONFIG_SENSOR_LOG_LEVEL);

struct led_sensor_config {
	struct gpio_dt_spec led;
	uint32_t init_delay_ms;
};

/* Mutable, estado en runtime */
struct led_sensor_data {
	bool led_on;
	uint32_t fetch_count;
	uint32_t blink_delay_ms;
};

static int led_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct led_sensor_data *data = dev->data;
	const struct led_sensor_config *cfg = dev->config;

	if (chan != SENSOR_CHAN_ALL) {
		return -ENOTSUP;
	}

	gpio_pin_set_dt(&cfg->led, 1);
	data->led_on = true;
	data->fetch_count++;

	LOG_DBG("fetch #%u: LED ON", data->fetch_count);
	return 0;
}

static int led_sensor_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	struct led_sensor_data *data = dev->data;
	const struct led_sensor_config *cfg = dev->config;

	if (chan != SENSOR_CHAN_ALL) {
		return -ENOTSUP;
	}

	gpio_pin_set_dt(&cfg->led, 0);
	data->led_on = false;

	val->val1 = data->led_on ? 1 : 0;
	val->val2 = 0;

	LOG_DBG("channel_get: LED OFF");
	return 0;
}

int led_sensor_set_blink_delay(const struct device *dev, uint32_t delay_ms)
{
	struct led_sensor_data *data = dev->data;

	data->blink_delay_ms = delay_ms;
	LOG_DBG("Blink delay set to %u ms", delay_ms);
	return 0;
}

int led_sensor_get_blink_delay(const struct device *dev, uint32_t *delay_ms)
{
	struct led_sensor_data *data = dev->data;

	if (delay_ms == NULL) {
		return -EINVAL;
	}

	*delay_ms = data->blink_delay_ms;
	LOG_DBG("Blink delay retrieved: %u ms", *delay_ms);
	return 0;
}

static DEVICE_API(sensor, led_sensor_api) = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get = led_sensor_channel_get,
};

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		LOG_ERR("GPIO not ready");
		return -ENODEV;
	}

	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define LED_SENSOR_DEFINE(inst)                                       \
	static struct led_sensor_data data_##inst;                      \
	static const struct led_sensor_config cfg_##inst = {            \
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios),              \
	};                                                              \
	DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL,              \
			      &data_##inst, &cfg_##inst,              \
			      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, \
			      &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)