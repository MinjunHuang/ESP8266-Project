#include "user_config.h"
#include "dht.h"

#include <ets_sys.h>
#include <osapi.h>
#include <ip_addr.h>
#include <user_interface.h>
#include <espconn.h>
#include <mem.h>

static struct dht_t dht;

struct station_config stationConf;

void connect_cb(void *arg);
void disconnect_cb(void *arg);
void reconnect_cb(void *arg, int8_t errno);
void wifi_handle_event_cb(System_Event_t *evt);
void wifi_connect_cb();
void data_sent_callback(void *arg);

void data_sent_callback(void *arg)
{
	os_printf("Data sent successfully\n");
    struct espconn *conn = (struct espconn *)arg;
	espconn_disconnect(conn);
}


/**
 * void wifi_connect_cb()
 * This function is called when the chip successfully
 * connects to the AP.
 * The code in it attempts to connect to the server.
 */
void wifi_connect_cb()
{
	os_printf("Connected to AP\n");
	
	// Setup espconn struct for connection to the server
	struct espconn *conn = 
		(struct espconn *)os_zalloc(sizeof(struct espconn));
	conn->type = ESPCONN_TCP;
	conn->state = ESPCONN_NONE;
	conn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	conn->proto.tcp->local_port = espconn_port();
	conn->proto.tcp->remote_port = SERVER_PORT;
	SET_IP(conn->proto.tcp->remote_ip, SERVER_IP);

	// Attempt to connect to server
	if(espconn_connect(conn)) {
		os_printf("wifi_connect_cb::espconn_connect() failed\n");
		system_deep_sleep(SLEEP_TIME);
	} else {
		// Register callbacks for connection
	    espconn_regist_connectcb(conn, connect_cb);
    	espconn_regist_disconcb(conn, disconnect_cb);
		espconn_regist_reconcb(conn, reconnect_cb);
	}
}

/**
 * void connect_cb()
 * Called when the TCP connection to the server is established
 */
void connect_cb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;

	// Read from sensor
	float temp, hum;
	if (dht_read_temperature(&dht, &temp))
		os_printf("Error while reading temperature: %s\n",
				dht_state_string(&dht));
	if (dht_read_humidity(&dht, &hum))
		os_printf("Error while reading humidity: %s\n",
				dht_state_string(&dht));

	os_printf("Temp: %d; Humidity: %d\n", (int32)temp, (int32)hum);

	char request[255]; // The HTTP request data
	char body[127]; // Only the body of the request

	// Add data to the request body
	os_sprintf(body, "temperature=%d&humidity=%d",
			(int32)temp, (int32)hum);
	uint8_t body_len = strlen(body); // The length of the body

	// Fill the request
	os_sprintf(request, ""
			"POST /api/list HTTP/1.1\r\n"
			"Host: 192.168.97.16:8000\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: %d\r\n"
			"\r\n"
			"%s\r\n",
			body_len, body);

    if (espconn_send(conn, request, strlen(request))) {
		os_printf("connect_cb::espconn_send() failed. Aborting\n");
		system_deep_sleep(SLEEP_TIME);
	} else {
		// Setup callbacks [TODO add more]
		espconn_regist_sentcb(conn, data_sent_callback);
	}
}

/**
 * void reconnect_cb()
 * Called if connection to the server failed
 */
void reconnect_cb(void *arg, int8_t errno)
{
    os_printf("Failed to connect. Aborting\n");
	system_deep_sleep(SLEEP_TIME);
}

/**
 * void disconnect_cb()
 * Called after disconnecting from the server
 */
void disconnect_cb(void *arg)
{
    os_printf("Disconnected\n");
	system_deep_sleep(SLEEP_TIME);
}



/**
 * void wifi_handle_event_cb()
 * Callback for wifi events
 * Used to call wifi_connect_cb()
 */
void wifi_handle_event_cb(System_Event_t *evt)
{
	switch(evt->event) {
	case EVENT_STAMODE_GOT_IP:
		wifi_connect_cb();
		break;
	}
}

void user_init()
{
	// Configure wifi
	const char ssid[32] = SSID;
	const char pass[32] = PASS;
	wifi_set_event_handler_cb(wifi_handle_event_cb);
	wifi_set_opmode( STATION_MODE );
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, pass, 32);
	wifi_station_set_config(&stationConf);
	wifi_station_connect();

	// Configure UART for debugging
    uart_div_modify(0, UART_CLK_FREQ / UART_BAUD);
	os_delay_us(100000);

	// Initialize the dht
	dht_init(&dht, DHT_PIN);
}