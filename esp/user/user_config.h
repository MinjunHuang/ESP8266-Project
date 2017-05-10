#ifndef USER_CONFIG_H_INCLUDED
#define USER_CONFIG_H_INCLUDED

// Sleep
#define SLEEP_TIME 30*60*1000000

// Server
#define SERVER_PORT 8000
#define SERVER_IP 192, 168, 0, 40

// Some quantum magic
#define SET_IP(ptr, ...) _SET_IP(ptr, __VA_ARGS__)
#define _SET_IP(ptr, i0, i1, i2, i3) \
	ptr[0] = i0; ptr[1] = i1; \
	ptr[2] = i2; ptr[3] = i3

// AP
#define SSID "ssid"
#define PASS "pass"

// UART
#define UART_BAUD 9600

// DHT
#define DHT_PIN 5

#endif
