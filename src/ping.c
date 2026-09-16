/*
 * Board A ("pinger"): sends a PING every 5 s, listens briefly for a PONG,
 * then lets the SoC drop into Stop2 for the rest of the period. No
 * app-level PM calls are needed -- with CONFIG_PM=y the idle thread picks
 * the deepest residency-appropriate power state automatically whenever
 * nothing is runnable (i.e. during k_sleep()).
 */
#include "ping.h"

LOG_MODULE_REGISTER(pinger, LOG_LEVEL_INF);

static const struct device *lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));

static int configure_for_tx(uint16_t preamble_symbols)
{
	struct lora_modem_config cfg = {
		.frequency = LORA_FREQUENCY_HZ,
		.bandwidth = LORA_BANDWIDTH,
		.datarate = LORA_SPREADING_FACT,
		.coding_rate = LORA_CODING_RATE,
		.preamble_len = preamble_symbols,
		.tx_power = LORA_TX_POWER_DBM,
		.tx = true,
	};

	return lora_config(lora_dev, &cfg);
}

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

int main_ping(void)
{
	uint32_t seq = 0;

	if (!device_is_ready(lora_dev)) {
		LOG_ERR("LoRa device not ready");
		return -ENODEV;
	}

	while (1) {
		struct ping_pong_pkt tx_pkt = {
			.magic = PKT_MAGIC,
			.type = PKT_PING,
			.seq = seq++,
		};
		int ret;

		/*
		 * Long preamble so Board B's hardware RX duty cycle
		 * (rx=50 ms / sleep=450 ms) is guaranteed to catch it
		 * during one of its listen windows. See proto.h.
		 */
		ret = configure_for_tx(PING_PREAMBLE_SYMBOLS);
		if (ret < 0) {
			LOG_ERR("lora_config (tx) failed: %d", ret);
			goto sleep;
		}

		LOG_INF("Sending PING seq=%u", tx_pkt.seq);
		ret = lora_send(lora_dev, (uint8_t *)&tx_pkt, sizeof(tx_pkt));
		if (ret < 0) {
			LOG_ERR("lora_send failed: %d", ret);
			goto sleep;
		}

		/* Switch to RX and listen briefly for Board B's reply. */
		ret = configure_for_rx();
		if (ret < 0) {
			LOG_ERR("lora_config (rx) failed: %d", ret);
			goto sleep;
		}

		uint8_t rx_buf[32];
		int16_t rssi;
		int8_t snr;

		ret = lora_recv(lora_dev, rx_buf, sizeof(rx_buf), K_MSEC(PONG_LISTEN_MS), &rssi,
				&snr);
		if (ret > 0) {
			struct ping_pong_pkt *rx_pkt = (struct ping_pong_pkt *)rx_buf;

			if (ret >= (int)sizeof(*rx_pkt) && rx_pkt->magic == PKT_MAGIC &&
			    rx_pkt->type == PKT_PONG) {
				LOG_INF("Got PONG seq=%u rssi=%d snr=%d", rx_pkt->seq, rssi, snr);
			} else {
				LOG_WRN("Ignoring unrecognized %d-byte reply", ret);
			}
		} else if (ret == -EAGAIN || ret == 0) {
			LOG_WRN("No PONG received (timeout)");
		} else {
			LOG_ERR("lora_recv error: %d", ret);
		}

sleep:
		/*
		 * k_sleep() lets the idle thread pick Stop0/1/2 depending on
		 * residency. This is where nearly all of the power saving
		 * for Board A comes from.
		 */
		k_sleep(K_SECONDS(PING_PERIOD_S));
	}

	return 0;
}
