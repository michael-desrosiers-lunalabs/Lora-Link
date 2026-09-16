/*
 * Board B ("ponger"): the radio autonomously duty-cycles between a short
 * RX window and sleep (lora_recv_duty_cycle()). The calling thread blocks
 * the whole time; the SoC can drop into Stop2 and only comes back when the
 * radio's own IRQ fires on a detected packet -- that's the actual
 * "deep sleep except when woken by received data" behavior.
 *
 * NOTE: lora_recv_duty_cycle()/_async() are a fairly recent addition to
 * zephyr/drivers/lora.h. If your Zephyr checkout predates it, this file
 * won't build -- fall back to the plain lora_recv(..., K_FOREVER, ...)
 * version described in README.md (continuous RX; still lets the CPU
 * sleep, but the radio itself burns full RX current the whole time
 * instead of duty-cycling).
 */

#include "pong.h"

LOG_MODULE_REGISTER(ponger, LOG_LEVEL_INF);

static const struct device *lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));

static int configure_for_rx(void)
{
	struct lora_modem_config cfg = {
		.frequency = LORA_FREQUENCY_HZ,
		.bandwidth = LORA_BANDWIDTH,
		.datarate = LORA_SPREADING_FACT,
		.coding_rate = LORA_CODING_RATE,
		.preamble_len = DEFAULT_PREAMBLE_SYMBOLS,
		.tx_power = LORA_TX_POWER_DBM,
		.tx = false,
	};

	return lora_config(lora_dev, &cfg);
}

static int configure_for_tx(void)
{
	struct lora_modem_config cfg = {
		.frequency = LORA_FREQUENCY_HZ,
		.bandwidth = LORA_BANDWIDTH,
		.datarate = LORA_SPREADING_FACT,
		.coding_rate = LORA_CODING_RATE,
		.preamble_len = DEFAULT_PREAMBLE_SYMBOLS,
		.tx_power = LORA_TX_POWER_DBM,
		.tx = true,
	};

	return lora_config(lora_dev, &cfg);
}

int main_pong(void)
{
	int ret;

	if (!device_is_ready(lora_dev)) {
		LOG_ERR("LoRa device not ready");
		return -ENODEV;
	}

	ret = configure_for_rx();
	if (ret < 0) {
		LOG_ERR("lora_config (rx) failed: %d", ret);
		return ret;
	}

	LOG_INF("Ponger ready: duty-cycled RX (rx=%dms sleep=%dms)", DUTY_RX_WINDOW_MS,
		DUTY_SLEEP_WINDOW_MS);

	while (1) {
		uint8_t rx_buf[32];
		int16_t rssi;
		int8_t snr;

		ret = lora_recv_duty_cycle(lora_dev, K_MSEC(DUTY_RX_WINDOW_MS),
					   K_MSEC(DUTY_SLEEP_WINDOW_MS), rx_buf, sizeof(rx_buf),
					   K_FOREVER, &rssi, &snr);
		if (ret <= 0) {
			LOG_WRN("lora_recv_duty_cycle error: %d", ret);
			configure_for_rx();
			continue;
		}

		struct ping_pong_pkt *rx_pkt = (struct ping_pong_pkt *)rx_buf;

		if (ret < (int)sizeof(*rx_pkt) || rx_pkt->magic != PKT_MAGIC ||
		    rx_pkt->type != PKT_PING) {
			LOG_WRN("Ignoring unrecognized %d-byte packet", ret);
			configure_for_rx();
			continue;
		}

		LOG_INF("Got PING seq=%u rssi=%d snr=%d", rx_pkt->seq, rssi, snr);

		struct ping_pong_pkt tx_pkt = {
			.magic = PKT_MAGIC,
			.type = PKT_PONG,
			.seq = rx_pkt->seq,
		};

		ret = configure_for_tx();
		if (ret < 0) {
			LOG_ERR("lora_config (tx) failed: %d", ret);
			continue;
		}

		ret = lora_send(lora_dev, (uint8_t *)&tx_pkt, sizeof(tx_pkt));
		if (ret < 0) {
			LOG_ERR("lora_send (pong) failed: %d", ret);
		}

		/* Back to duty-cycled RX for the next ping. */
		configure_for_rx();
	}

	return 0;
}
