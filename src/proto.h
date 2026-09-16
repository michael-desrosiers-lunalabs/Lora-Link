#ifndef APP_PROTO_H
#define APP_PROTO_H

#include <stdint.h>

/*
 * Radio parameters shared by both boards. Both ends MUST agree on
 * frequency/bandwidth/SF/CR or they won't hear each other.
 *
 * 868.1 MHz / SF7 / 125 kHz / CR 4:5 is a common EU868 "LoRaWAN-like"
 * single-channel config. Swap to 915 MHz (e.g. 915000000) and check your
 * local ISM-band rules if you're in a US915 region -- the LoRa-E5 module
 * is sold in HF (863-928 MHz) and LF (410-493 MHz) variants, so make sure
 * this frequency is actually inside the band your module covers.
 */
#define LORA_FREQUENCY_HZ   868100000
#define LORA_BANDWIDTH      BW_125_KHZ
#define LORA_SPREADING_FACT SF_7
#define LORA_CODING_RATE    CR_4_5
#define LORA_TX_POWER_DBM   14

/*
 * Board A's ping preamble must be long enough to guarantee Board B's
 * duty-cycled receiver is awake and listening somewhere during it:
 *
 *   preamble_symbols > (rx_period_ms + sleep_period_ms) / symbol_time_ms
 *
 * symbol_time_ms = (2^SF / BW_hz) * 1000. For SF7/125kHz that's ~1.024 ms.
 * With rx=50ms / sleep=450ms (500ms duty cycle period) that needs a
 * preamble of >489 symbols. Recompute this if you change SF/BW/duty
 * cycle periods -- see README.md.
 */
#define PING_PREAMBLE_SYMBOLS  600
#define DEFAULT_PREAMBLE_SYMBOLS 8   /* used for the PONG reply */

#define PING_PERIOD_S        5
#define PONG_LISTEN_MS        2000  /* how long A listens for a PONG */

/* Board B's hardware RX duty cycle (radio-autonomous, not CPU-driven) */
#define DUTY_RX_WINDOW_MS      50
#define DUTY_SLEEP_WINDOW_MS  450

#define PKT_MAGIC 0x50 /* 'P' */

enum pkt_type {
	PKT_PING = 1,
	PKT_PONG = 2,
};

struct __packed ping_pong_pkt {
	uint8_t magic;
	uint8_t type;
	uint32_t seq;
};

#endif /* APP_PROTO_H */