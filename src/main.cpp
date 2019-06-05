#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "config.h"

#include <IFTTTWebhook.h>
IFTTTWebhook ifttt(IFTTT_API_KEY, IFTTT_TRIGGER_NAME);

#include "OpenWeatherMap.h"

OpenWeatherMap owm(OPEN_WEATHERMAP_API_KEY, OPEN_WEATHERMAP_CITY_CODE);

#include <PubSubClient.h>
static WiFiClient wifi_mqtt_client;
static PubSubClient mqtt_client(wifi_mqtt_client);

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme280;

#include "ui.h"

// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// GxIO_Class  io(SPI, CS, MOSI, RST, BL);
// GxEPD_Class display(io, rst, busy);
GxIO_Class     io(SPI, SS, 17, 16);
GxEPD_Class    display(io, 16, 4);

ClockUI clock_ui(&display);

#include "uptime.h"

Uptime uptime;

#define AMBIENT_LIGHT_PIN 35

#include <rom/rtc.h>

const char* reboot_reason(int code) {
  switch(code) {
    case 1 : return "POWERON_RESET";          /**<1, Vbat power on reset*/
    case 3 : return "SW_RESET";               /**<3, Software reset digital core*/
    case 4 : return "OWDT_RESET";             /**<4, Legacy watch dog reset digital core*/
    case 5 : return "DEEPSLEEP_RESET";        /**<5, Deep Sleep reset digital core*/
    case 6 : return "SDIO_RESET";             /**<6, Reset by SLC module, reset digital core*/
    case 7 : return "TG0WDT_SYS_RESET";       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : return "TG1WDT_SYS_RESET";       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : return "RTCWDT_SYS_RESET";       /**<9, RTC Watch dog Reset digital core*/
    case 10 : return "INTRUSION_RESET";       /**<10, Instrusion tested to reset CPU*/
    case 11 : return "TGWDT_CPU_RESET";       /**<11, Time Group reset CPU*/
    case 12 : return "SW_CPU_RESET";          /**<12, Software reset CPU*/
    case 13 : return "RTCWDT_CPU_RESET";      /**<13, RTC Watch dog Reset CPU*/
    case 14 : return "EXT_CPU_RESET";         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : return "RTCWDT_BROWN_OUT_RESET";/**<15, Reset when the vdd voltage is not stable*/
    case 16 : return "RTCWDT_RTC_RESET";      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : return "NO_MEAN";
  }
}

WiFiMulti wifiMulti;

unsigned long start_showing_clock = 0;

#ifdef BUILD_INFO

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

static char build_info[] = STRINGIZE(BUILD_INFO);
#else
static char build_info[] = "not set";
#endif

static char hostname[sizeof("eink-clock-%02x%02x%02x") + 1];

static RTC_DATA_ATTR int bootCount = 0;
static RTC_DATA_ATTR int wifi_failures = 0;

void command_callback(const char* topic, byte* payload, unsigned int length);

void setup() {
  byte mac_address[6];

  bootCount++;
  
  delay(500);

  randomSeed(analogRead(0));
  Serial.begin(115200);
  Serial.println("Hello world");

  WiFi.macAddress(mac_address);
  snprintf(hostname, sizeof(hostname), "eink-clock-%02x%02x%02x", (int)mac_address[3], (int)mac_address[4], (int)mac_address[5]);
  Serial.printf("Hostname is %s\n", hostname);

  WiFi.setHostname(hostname);

  Serial.println("Starting...");
  clock_ui.begin();
  delay(1000);
  Serial.println("eink setup done");

#if 0
  clock_ui.test();
  Serial.println("test done");
  delay(100000);
#endif

  Serial.println("[wifi]");

  wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);
  wifiMulti.run();

  //  clock_ui.show_wifi_symbol();
  
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.print("Local IP ");
  Serial.println(WiFi.localIP());

  if(!MDNS.begin(hostname))
    Serial.println("Error setting up MDNS responder!");
  else
    Serial.println("[mDNS]");

  Serial.println("[IFTTT]");
  ifttt.trigger("reboot", reboot_reason(rtc_get_reset_reason(0)),  reboot_reason(rtc_get_reset_reason(1)));


  Serial.println("[NTP]");
  configTime(TZ_OFFSET, DST_OFFSET, "pool.ntp.org", "time.nist.gov");

  Serial.println("[OTA]");
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      ifttt.trigger("update", "start", type.c_str());

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      ifttt.trigger("update", "end");
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
	char buffer[10];
	snprintf(buffer, 10, "%u", error);
	ifttt.trigger("update", "error", buffer);

      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });


  clock_ui.clear_wifi_symbol();

  clock_ui.show_network_info(WiFi.localIP(), HOSTNAME, WiFi.SSID().c_str());

  //  start_showing_clock = millis() + 30 * 1000;
  start_showing_clock = millis() + 1 * 1000;

#if 0
  display.update();
#endif

  ArduinoOTA.begin();

  mqtt_client.setServer(MQTT_HOST, MQTT_PORT);
  mqtt_client.connect(MQTT_UUID, MQTT_USER, MQTT_PASS);
  mqtt_client.setCallback(command_callback);
  mqtt_client.subscribe("/eink-clock/cmd");

  Serial.println("[homebus]");
  Serial.println("[mqtt]");

  if(!bme280.begin(0x76)) {
    Serial.println("bme280 fail");
  }
  Serial.println("[bme280]");

  Serial.println("[setup done]");
}

void update_time() {
  //   uint16_t cursor_y = time_box.y + 16;
  struct tm timeinfo;
  time_t now;

  now = time(NULL);
  localtime_r(&now, &timeinfo);

  clock_ui.show_time(&timeinfo);

  Serial.println(asctime(&timeinfo));
}

void owm_conditions() {
  static float last_outdoor_tempearture = 0;
  static float last_outdoor_humidity = 0;

  OpenWeatherMapConditions current = owm.current();

  clock_ui.show_outdoor(current.temp, current.humidity);
}

static float last_indoor_temperature = 0;
static float last_indoor_humidity = 0;

void update_indoor_conditions() {
  if(last_indoor_temperature && last_indoor_humidity)
    clock_ui.show_indoor(last_indoor_temperature, last_indoor_humidity);
}

#define CLOCK 0
#define REHAB 1
static int mode = CLOCK;

void command_callback(const char* topic, byte* payload, unsigned int length) {
  char payload_str[length+1];
  memcpy(payload_str, payload, length);
  payload_str[length] = '\0';

  Serial.printf("command: topic %s, payload %s\n", topic, payload_str);

  if(strcmp(payload_str, "restart") == 0)
    ESP.restart();

  if(strcmp(payload_str, "clock") == 0) {
    Serial.println("clock mode");
    mode = CLOCK;
  }

  if(strcmp(payload_str, "rehab") == 0) {
    Serial.println("rehab mode");
    mode = REHAB;
  }
}

void loop_rehab() {
  static int submode = 0;
  static unsigned long next_rehab_update = 0;

  if(next_rehab_update > millis())
    return;

  switch(submode) {
  case 0:
    display.fillScreen(GxEPD_WHITE);
    display.update();
    submode++;
    break;
  case 1:
    display.fillScreen(GxEPD_BLACK);
    display.update();
    submode++;
    break;
  case 2:
    display.fillScreen(GxEPD_WHITE);
    display.update();
    submode++;
    break;
  case 3:
    for(int x = 0; x < 400;  x += 10)
      for(int y = 0; y < 300; y += 10) {
	for(int i = 0; i < 5; i++)
	  for(int j = 0; j < 5 ; j++)
	    display.drawPixel(x+i, y+j, GxEPD_WHITE);

	for(int i = 0; i < 5; i++)
	  for(int j = 0; j < 5 ; j++)
	    display.drawPixel(x+i+5, y+j+5, GxEPD_BLACK);
      }
    display.update();
    submode++;
    break;
  case 4:
    for(int x = 0; x < 400;  x += 10)
      for(int y = 0; y < 300; y += 10) {
	for(int i = 0; i < 5; i++)
	  for(int j = 0; j < 5 ; j++)
	    display.drawPixel(x+i, y+j, GxEPD_BLACK);

	for(int i = 0; i < 5; i++)
	  for(int j = 0; j < 5 ; j++)
	    display.drawPixel(x+i+5, y+j+5, GxEPD_WHITE);
      }
    display.update();
    submode++;
    break;
  case 5:
    submode = 0;
    return;
  };

  next_rehab_update += 3*60*1000;
}

void loop_clock() {
  static unsigned long next_weather_update = 0;
  static uint8_t draw_clock_ui = 1;

  if(draw_clock_ui) {
    draw_clock_ui = 0;
    clock_ui.setup_time();
  }

  static tm last_timeinfo = {
    .tm_sec = -1,
    .tm_min = -1
  };

  struct tm timeinfo;
  time_t now;

  now = time(NULL);
  localtime_r(&now, &timeinfo);

#ifdef CHECK_AMBIENT_LIGHT
  if(last_timeinfo.tm_sec != timeinfo.tm_sec && ambient_light > 0) {
#else
  if(last_timeinfo.tm_sec != timeinfo.tm_sec) {
#endif
    update_time();
    last_timeinfo = timeinfo;
  }

#ifdef CHECK_AMBIENT_LIGHT
  if(millis() > next_weather_update && ambient_light > 0) {
#else
  if(millis() > next_weather_update) {
#endif
    next_weather_update += 1000 * 60;

    owm.update_current();
    owm_conditions();
  }
}

void loop() {
  static unsigned long last_mqtt_check = 0;

  unsigned ambient_light = analogRead(AMBIENT_LIGHT_PIN);

  mqtt_client.loop();

  if(millis() > last_mqtt_check + 5000) {
    if(!mqtt_client.connected()) {
      mqtt_client.connect(MQTT_UUID, MQTT_USER, MQTT_PASS);
      mqtt_client.subscribe("/eink-clock/cmd");
      Serial.println("mqtt reconnect");
    }

    last_mqtt_check = millis();
  }

  ArduinoOTA.handle();

  if(mode == REHAB)
    loop_rehab();

  if(millis() < start_showing_clock)
    return;

  if(mode ==  CLOCK)
    loop_clock();

  static unsigned long next_bme280_update = 1000*10;
  if(millis() > next_bme280_update) {
    bool redraw = false;

    next_bme280_update = millis() + 1000*10;

    if(last_indoor_temperature != bme280.readTemperature()) {
      last_indoor_temperature = bme280.readTemperature();
      redraw = true;
    }

    if(last_indoor_humidity != bme280.readHumidity()) {
      last_indoor_humidity = bme280.readHumidity();
      redraw = true;
    }
    
#ifdef CHECK_AMBIENT_LIGHT
    if(mode == CLOCK && redraw && ambient_light > 0)
#else
    if(mode == CLOCK && redraw)
#endif
      update_indoor_conditions();

    Serial.print("BME280 Temperature = ");
    Serial.print(last_indoor_temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme280.readPressure());
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme280.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(last_indoor_humidity);
    Serial.println(" %");

    Serial.printf("Ambient light %u\n", ambient_light);

    Serial.println();

    static unsigned long last_mqtt_update = 0;
    if(millis() - last_mqtt_update > (60 * 1000))  {
      last_mqtt_update = millis();
      char buffer[500];
      IPAddress local = WiFi.localIP();

      static bool first = true;
      if(first) {
	first = false;
	snprintf(buffer, 500, "{ \"id\": \"%s\", \"message\": \"booted\", \"system\": { \"name\": \"%s\", \"build\": \"%s\", \"ip\": \"%d.%d.%d.%d\", \"rssi\": %d } }",
		 MQTT_UUID,
		 hostname, build_info, local[0], local[1], local[2], local[3], WiFi.RSSI());
	mqtt_client.publish("/eink-clock/log", buffer, true);
      }

      snprintf(buffer, 500, "{ \"id\": \"%s\", \"system\": { \"name\": \"%s\", \"build\": \"%s\", \"freeheap\": %d, \"uptime\": %lu, \"ip\": \"%d.%d.%d.%d\", \"rssi\": %d, \"reboots\": %d, \"wifi_failures\": %d  }, \"environment\": { \"temperature\": %0.1f, \"humidity\": %0.1f, \"pressure\": %0.1f },  \"light\": %u }",
	       MQTT_UUID,
	       hostname, build_info, ESP.getFreeHeap(), uptime.uptime()/1000,  local[0], local[1], local[2], local[3], WiFi.RSSI(), bootCount, wifi_failures,
	       last_indoor_temperature, bme280.readHumidity(), bme280.readPressure()/100.0,
	       ambient_light);

      Serial.println(buffer);
      mqtt_client.publish("/eink-clock", buffer, true);
    }
  }
}
