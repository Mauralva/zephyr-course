#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>

#define LED_SENSOR_NODE DT_NODELABEL(my_led_sensor)

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(LED_SENSOR_NODE);
	struct sensor_value val;

	if (!device_is_ready(dev)) {
		printk("LED sensor device not ready\n");
		return 0;
	}
	printk("Device %s ready\n", dev->name);

	while (1) {
		/* Tarea 1: fetch enciende, channel_get apaga */
		sensor_sample_fetch(dev);
		k_msleep(500);

		sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
		k_msleep(500);
	}
}