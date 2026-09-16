#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/logging/log.h>

#include "proto.h"

int main_pong(void);
static int configure_for_tx(void);
static int configure_for_rx(void);
