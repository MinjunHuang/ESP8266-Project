#include "dht.h"
#include <osapi.h>
#include <ets_sys.h>
#include <gpio.h>

void dht_init(struct dht_t *dht, uint8 gpio_name)
{
	dht->gpio_name = gpio_name;
	dht->state = DHT_OK;
	dht->_last_poll = -1;
}

uint8 dht_read_temperature(struct dht_t *dht, float *temperature)
{
	uint32 current_time = system_get_time();
	if (current_time - dht->_last_poll >= DHT_POLL_INTERVAL ||
		dht->_last_poll < 0)
		dht_force_poll(dht);
	if (temperature)
		*temperature = dht->_last_temperature;
	return dht->state;
}

uint8 dht_read_humidity(struct dht_t *dht, float *humidity)
{
	uint32 current_time = system_get_time();
	if (current_time - dht->_last_poll >= DHT_POLL_INTERVAL ||
		dht->_last_poll < 0)
		dht_force_poll(dht);
	if (humidity)
		*humidity = dht->_last_humidity;
	return dht->state;
}

// Expect pin 'pin' to be at level 'level' for a certain
// period of time and return the number of loop cycles
// spent at this level. Can be used to compare the relative
// length of two pulses. Returns 0 on time-out.
static uint32 expect_pulse(uint8 pin, uint32 level)
{
	uint32 cycles = 0;
	uint32 current_level;
	for (;;) {
		current_level = GPIO_INPUT_GET(pin);
		if (current_level == level)
			break;
		++cycles;
		if (cycles > DHT_MAX_CYCLES)
			return 0;
	}
	cycles = 1;
	for (;;) {
		current_level = GPIO_INPUT_GET(pin);
		if (current_level != level)
			return cycles;
		++cycles;
		if (cycles > DHT_MAX_CYCLES)
			return 0;
	}
}

uint8 dht_force_poll(struct dht_t *dht)
{
	uint8 pin = GPIO_ID_PIN(dht->gpio_name);
	dht->_last_poll = system_get_time();

	// Set data level to HIGH for 250ms
	GPIO_OUTPUT_SET(pin, 1);
	os_delay_us(250000);

	// Set data level to LOW for 20ms
	GPIO_OUTPUT_SET(pin, 0);
	os_delay_us(20000);

	// Set data level to HIGH for 40us
	GPIO_OUTPUT_SET(pin, 1);
	os_delay_us(40);

	// Turn off interrupts, because section is timing-critical
	ets_intr_lock();

	// Set pin as INPUT
	GPIO_DIS_OUTPUT(pin);
	os_delay_us(10);

	// First expect a LOW signal for ~80us
	// followed by a HIGH signal for ~80us
	if (expect_pulse(pin, 0) == 0) {
		dht->state = DHT_TIMEOUT;
		return dht->state;;
	}
	if (expect_pulse(pin, 1) == 0) {
		dht->state = DHT_TIMEOUT;
		return dht->state;
	}

	// Read 40 bits, sent by the sensor. Each bit is sent
	// as a ~50us LOW pulse, followed by a variable-length
	// HIGH pulse. If the HIGH pulse is ~28us, then it's a
	// 0 and if it's ~70us then it's a 1
	// We compare the number of cycles spent in LOW
	// to the number of cycles spent in HIGH
	uint32 cycles[80];
	uint8 i;
	for (i = 0; i < 80; i += 2) {
		cycles[i] = expect_pulse(pin, 0);
		cycles[i+1] = expect_pulse(pin, 1);
	}

	// End of timing-critical section
	ets_intr_unlock();

	// Received data is stored here
	uint32 data[5] = {0, 0, 0, 0, 0};
	// Inspect the pulses and determine which is 0/1
	for (i = 0; i < 80; i += 2) {
		uint32 low_cycles = cycles[i];
		uint32 high_cycles = cycles[i+1];

		if (!low_cycles || !high_cycles) {
			dht->state = DHT_TIMEOUT;
			return dht->state;
		}
		data[i/16] <<= 1;
		// If the HIGH cycle count if higher
		// then it should be a 1
		if (high_cycles > low_cycles) {
			data[i/16] |= 1;
		}
		// Else it should be a 0 (Nothing to do)
	}

	// Verify the checksum
	uint32 checksum = ((data[0] + data[1] + data[2] + data[3]) & 0xFF);
	if (data[4] != checksum) {
		// Corrupted data
		dht->state = DHT_CHECKSUM_ERROR;
		return dht->state;
	}

	// Calculate temperature
	float temp = (data[2] & 0x7F);
	temp *= 256;
	temp += data[3];
	temp *= 0.1f;
	if (data[2] & 0x80)
		temp = -temp;
	dht->_last_temperature = temp;

	// Calculate himudity
	float hum = data[0];
	hum *= 256;
	hum += data[1];
	hum *= 0.1;
	dht->_last_humidity = hum;

	dht->state = DHT_OK;
	return dht->state;
}

char* dht_state_string(struct dht_t *dht)
{
	switch (dht->state) {
	case DHT_OK: return "OK";
	case DHT_TIMEOUT: return "TIMEOUT";
	case DHT_CHECKSUM_ERROR: return "CHECKSUM_ERROR";
	default: return "UNKNOWN_ERROR";
	}
}

