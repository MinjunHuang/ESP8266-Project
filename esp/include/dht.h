#ifndef DHT_H_INCLUDED
#define DHT_H_INCLUDED

#define DHT_MAX_CYCLES 1000
#define DHT_POLL_INTERVAL 2000000

#include <c_types.h>

enum DHT_STATE {
	DHT_OK = 0,
	DHT_TIMEOUT,
	DHT_CHECKSUM_ERROR
};

struct dht_t {
	uint8 gpio_name;
	uint8 state;
	float _last_temperature;
	float _last_humidity;
	int32 _last_poll;
};

void dht_init(struct dht_t *dht, uint8 gpio_name);
uint8 dht_read_temperature(struct dht_t *dht, float *temperature);
uint8 dht_read_humidity(struct dht_t *dht, float *humidity);
uint8 dht_force_poll(struct dht_t *dht);

#endif
