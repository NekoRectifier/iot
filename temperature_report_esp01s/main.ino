#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <ESP8266WiFi.h>
#include <DFRobot_DHT11.h>
#include <ESP8266WiFiMulti.h>

#define LOG_D(fmt, ...) printf_P(PSTR(fmt "\n"), ##__VA_ARGS__);

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_temperature;
extern "C" homekit_characteristic_t cha_humidity;

static uint32_t next_heap_millis = 0;
static uint32_t next_report_millis = 0;
DFRobot_DHT11 DHT;

void setup()
{
	Serial.begin(115200);
	init_wifi();
	// homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
	arduino_homekit_setup(&config);
}

void loop()
{
	arduino_homekit_loop();
	const uint32_t t = millis();
	if (t > next_report_millis)
	{
		// report sensor values every 15 seconds
		next_report_millis = t + 15 * 1000;
		my_homekit_report();
	}

	delay(100);
}

void init_wifi()
{
	ESP8266WiFiMulti handle;
	handle.addAP("OpenWrt", "password");

	Serial.println("Connecting ...");
	int i = 0;
	while (handle.run() != WL_CONNECTED)
	{				 
		delay(50);
	}
}

// Called when the value is read by iOS Home APP
homekit_value_t cha_programmable_switch_event_getter()
{
    my_homekit_report();
	// Should always return "null" for reading, see HAP section 9.75
	return HOMEKIT_NULL_CPP();
}



void my_homekit_report()
{
	DHT.read(2); // using gpio2
	float t = DHT.temperature;
	float h = DHT.humidity;

	cha_temperature.value.float_value = t;
	homekit_characteristic_notify(&cha_temperature, cha_temperature.value);

	cha_humidity.value.float_value = h;
	homekit_characteristic_notify(&cha_humidity, cha_humidity.value);
	LOG_D("temp: %f, humid: %f", t, h);
}