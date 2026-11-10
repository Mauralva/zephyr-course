#ifndef CUSTOM_DRIVERS_LED_SENSOR_H_
#define CUSTOM_DRIVERS_LED_SENSOR_H_

#include <zephyr/device.h>

int led_sensor_set_blink_delay(const struct device *dev, uint32_t delay_ms);
int led_sensor_get_blink_delay(const struct device *dev, uint32_t *delay_ms);

#endif