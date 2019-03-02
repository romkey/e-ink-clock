#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "config.h"

#include <IFTTTWebhook.h>
IFTTTWebhook ifttt(IFTTT_API_KEY, IFTTT_TRIGGER_NAME);

#include "OpenWeatherMap.h"
OWMconditions      owCC;
OWMfiveForecast    owF5;
OWMsixteenForecast owF16;


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

void setup() {
  delay(500);

  randomSeed(analogRead(0));
  Serial.begin(115200);

  Serial.println("Starting...");
  clock_ui.begin();
  delay(1000);
  Serial.println("eink setup done");

#if 0
  clock_ui.test();
  Serial.println("test done");
  delay(100000);
#endif

#ifdef U8G2
  //  u8g2.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  //  u8g2.setFont(u8g2_font_logisoso92_tn);
  //  u8g2.setCursor(0,100);                // start writing at this position
  //  u8g2.print(F("123 Hello"));
  //  display.update();
  //  delay(2000);
#endif

  Serial.println("[wifi]");

  wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);

  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //  while(WiFi.status() != WL_CONNECTED) {
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.print("Local IP ");
  Serial.println(WiFi.localIP());

  Serial.println("[mdns]");
  MDNS.begin(HOSTNAME);

  Serial.println("[IFTTT]");
  ifttt.trigger("reboot", reboot_reason(rtc_get_reset_reason(0)),  reboot_reason(rtc_get_reset_reason(1)));

  Serial.println("[NTP]");
  configTime(TZ_OFFSET, DST_OFFSET, "pool.ntp.org", "time.nist.gov");

  Serial.println("[OTA]");
  ArduinoOTA.setHostname(HOSTNAME);
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
  OWM_conditions *ow_cond = new OWM_conditions;
  owCC.updateConditions(ow_cond, OPEN_WEATHERMAP_API_KEY, "us", "Portland", "english");

  Serial.print("Latitude & Longtitude: ");
  Serial.print("<" + ow_cond->longtitude + " " + ow_cond->latitude + "> @" + ow_cond->dt + ": ");
  Serial.println("icon: " + ow_cond->icon + ", " + " temp.: " + ow_cond->temp + ", press.: " + ow_cond->pressure);
}

static float last_indoor_temperature = 0;
static float last_indoor_humidity = 0;

void update_indoor_conditions() {
  if(last_indoor_temperature && last_indoor_humidity)
    clock_ui.show_indoor(last_indoor_temperature, last_indoor_humidity);
}

void loop() {
  static unsigned long next_weather_update = 0;
  static uint8_t draw_clock_ui = 1;

  ArduinoOTA.handle();

  if(millis() < start_showing_clock)
    return;

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

  if(last_timeinfo.tm_sec != timeinfo.tm_sec) {
    update_time();
    last_timeinfo = timeinfo;
  }

#if 0
  if(millis() > next_weather_update) {
    next_weather_update += 1000 * 60;

    owm_conditions();
  }
#endif


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
    
    if(redraw)
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

    Serial.println();
  }
}
