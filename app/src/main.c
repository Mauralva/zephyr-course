#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include "custom_drivers/led_sensor.h"


#define LED_SENSOR_NODE DT_NODELABEL(my_led_sensor)

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(LED_SENSOR_NODE);
	struct sensor_value val;
	uint32_t delay = 0;

	if (!device_is_ready(dev)) {
		printk("LED sensor device not ready\n");
		return 0;
	}
	printk("Device %s ready\n", dev->name);

	/* Tarea 2 */
	if (led_sensor_get_blink_delay(dev, &delay) == 0) {
		printk("Delay Inicial: %u ms\n", delay);
	}

	led_sensor_set_blink_delay(dev, 2000);

	if (led_sensor_get_blink_delay(dev, &delay) == 0) {
		printk("Nuevo delay: %u ms\n", delay);
	}

	while (1) {
		/* Lesson 7 Task 2*/
		uint32_t delay;
		
		led_sensor_get_blink_delay(dev, &delay);

		sensor_sample_fetch(dev);
		k_msleep(delay);

		sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
		k_msleep(delay);
	}
}