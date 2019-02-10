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

#ifdef U8G2
#include <U8g2_for_Adafruit_GFX.h>
U8G2_FOR_ADAFRUIT_GFX u8g2;
#endif


#include "ui.h"

// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// GxIO_Class  io(SPI, CS, MOSI, RST, BL);
// GxEPD_Class display(io, rst, busy);
GxIO_Class     io(SPI, SS, 17, 16);
GxEPD_Class    display(io, 16, 4);

const GFXfont* f = &FreeMonoBold24pt7b;


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
  delay(1000);

  display.init(115200);
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);

#ifdef U8G2
  u8g2.begin(display);
  Serial.println("u8g2 connected");

  u8g2.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2.setFontDirection(0);            // left to right (this is default)
  u8g2.setBackgroundColor(GxEPD_WHITE);      // apply Adafruit GFX color
  u8g2.setForegroundColor(GxEPD_BLACK);      // apply Adafruit GFX color
#endif

  clock_ui.show_wifi_symbol();
  display.update();

  Serial.println("eink setup done");
  delay(500);

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

  start_showing_clock = millis() + 30 * 1000;

#if 0
  display.update();
#endif

  ArduinoOTA.begin();

  Serial.println("[setup done]");
}

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} box_t;

void prep_box(box_t *box) {
  display.fillRect(box->x, box->y, box->w, box->h, GxEPD_WHITE);
  //  display.fillScreen(GxEPD_WHITE);
  //    u8g2.setCursor(box->x+4, box->y + 16 + 38);
#ifdef U8G2
  u8g2.setCursor(0, 100);
#else
  display.setCursor(box->x+4, box->y + 16 + 38);
#endif
}

#define update_box(box) display.updateWindow(box.x, box.y, box.w, box.h, true)

static box_t time_box = {
  .x = 5,
  .y = 10,
  .w = 380,
  .h = 100
};

static box_t day_box = {
};

static box_t date_box = {
};

static box_t weather_box = {
};

void update_time() {
  //   uint16_t cursor_y = time_box.y + 16;
  struct tm timeinfo;
  time_t now;

  now = time(NULL);
  localtime_r(&now, &timeinfo);

  prep_box(&time_box);
#ifdef U8G2
  u8g2.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
#else
  display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
#endif
  update_box(time_box);

  Serial.println(asctime(&timeinfo));
}


void update_date() {
}

void update_day() {
}

void owm_conditions() {
  OWM_conditions *ow_cond = new OWM_conditions;
  owCC.updateConditions(ow_cond, OPEN_WEATHERMAP_API_KEY, "us", "Portland", "english");

  Serial.print("Latitude & Longtitude: ");
  Serial.print("<" + ow_cond->longtitude + " " + ow_cond->latitude + "> @" + ow_cond->dt + ": ");
  Serial.println("icon: " + ow_cond->icon + ", " + " temp.: " + ow_cond->temp + ", press.: " + ow_cond->pressure);
}

void loop() {
  static unsigned long next_weather_update = 0;

  ArduinoOTA.handle();

  if(millis() < start_showing_clock)
    return;

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
}
