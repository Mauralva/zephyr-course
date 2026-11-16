#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include "custom_drivers/led_sensor.h"

#define LED_SENSOR_NODE DT_NODELABEL(my_led_sensor)

static const struct device *sensor_dev = DEVICE_DT_GET(LED_SENSOR_NODE);

/* sensor fetch — llama a sensor_sample_fetch() (prende el LED)*/
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(sensor_dev)) {
		shell_error(sh, "El sensor no está listo");
		return -ENODEV;
	}

	int err = sensor_sample_fetch(sensor_dev);

	if (err) {
		shell_error(sh, "Fallo sensor_sample_fetch (err: %d)", err);
		return err;
	}

	shell_print(sh, "Sensor fetched OK (LED ON)");
	return 0;
}

/* sensor read — llama a sensor_channel_get() e imprime el resultado (apaga el LED) */
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_value val;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(sensor_dev)) {
		shell_error(sh, "El sensor no está listo");
		return -ENODEV;
	}

	int err = sensor_channel_get(sensor_dev, SENSOR_CHAN_ALL, &val);

	if (err) {
		shell_error(sh, "Fallo sensor_channel_get (err: %d)", err);
		return err;
	}

	shell_print(sh, "Estado del LED: %s (val1=%d, val2=%d)",
		    val.val1 ? "ON" : "OFF", val.val1, val.val2);
	return 0;
}

/* sensor info — nombre del device y estado ready */
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "device: %s", sensor_dev->name);
	shell_print(sh, "ready:  %s", device_is_ready(sensor_dev) ? "si" : "no");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
	sub_sensor,
	SHELL_CMD(fetch, NULL, "Fetch LED (prende LED)", cmd_sensor_fetch),
	SHELL_CMD(read,  NULL, "Lee LED (apaga LED)", cmd_sensor_read),
	SHELL_CMD(info,  NULL, "Nombre y estado del sensor", cmd_sensor_info),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED Sensor Commands", NULL);