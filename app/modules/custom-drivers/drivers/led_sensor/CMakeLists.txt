if(CONFIG_CUSTOM_LED_SENSOR)
  zephyr_library()
  zephyr_library_sources(led_sensor.c)
endif()