#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* "custom,led-sensor" -> custom_led_sensor: vincula este .c con el nodo DT */
#define DT_DRV_COMPAT custom_led_sensor

LOG_MODULE_REGISTER(led_sensor, CONFIG_SENSOR_LOG_LEVEL);

/* Inmutable, fijado en build-time desde el DT (diapositiva 27) */
struct led_sensor_config {
	struct gpio_dt_spec led;
};

/* Mutable, estado en runtime */
struct led_sensor_data {
	bool led_on;
	uint32_t fetch_count;
};

/* sensor_sample_fetch: "habla con el hardware" -> enciende el LED */
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

/* sensor_channel_get: "entrega la muestra" -> apaga el LED y reporta estado */
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

/* Registrar la API estándar (diapositiva 31).
 * En Zephyr >= 4.1 debe envolverse con DEVICE_API() para que el subsistema
 * sensor valide la clase del dispositivo en runtime. */
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