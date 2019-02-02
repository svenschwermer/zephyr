zephyr_library()

zephyr_library_sources_ifdef(CONFIG_W1			w1.c)
zephyr_library_sources_ifdef(CONFIG_W1_GPIO		w1_gpio.c)
