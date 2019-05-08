#include "ui.h"

#include <Fonts/FreeMonoBold24pt7b.h>
#include "fonts/digital7mono967b.h"
#include "fonts/weather-icons-regular24pt7b.h"

#include "wifi_bmp.h"


static const GFXfont* f12 = &FreeMonoBold12pt7b;
static const GFXfont* f18 = &FreeMonoBold18pt7b;
static const GFXfont* f24 = &FreeMonoBold24pt7b;

static const GFXfont* f96 = &digital_7__mono_96pt7b;

static const GFXfont* w24 = &weathericons_regular_webfont24pt7b;

// screen is 300h x 400w

void ClockUI::begin() {
  _display->init(115200);
  _display->setRotation(0);
  _display->fillScreen(GxEPD_WHITE);
  _display->setTextColor(GxEPD_BLACK);

  show_wifi_symbol();
  _display->update();
}


void ClockUI::show_wifi_symbol() {
  _display->drawBitmap(wifi_symbol, 69, 19, 262, 193, GxEPD_BLACK);
}

void ClockUI::clear_wifi_symbol() {
  _display->fillRect(69, 19, 262, 193, GxEPD_WHITE);
  _display->updateWindow(69, 19, 262, 193);
}

void ClockUI::show_network_info(IPAddress local_ip, const char* name, const char* ssid) {
  _display->setTextSize(2);

  _display->fillRect(0, 0, 400, 180, GxEPD_WHITE);
  _display->setCursor(0, 10);

  _display->printf("Wifi: %s\n", ssid);

  _display->print("IP: ");
  _display->println(local_ip);
  
  _display->printf("Name: %s.local", name);

  _display->updateWindow(0, 0, 400, 100);
}

void ClockUI::clear_network_info() {
  _display->fillRect(10, 10, 200, 200, GxEPD_WHITE);
  _display->updateWindow(10, 10, 200, 200);
}

void ClockUI::setup_time() {
  _display->drawRect(0, 220, 400, 222, GxEPD_BLACK);
  _display->updateWindow(0, 220, 400, 222);
}

void ClockUI::show_time(struct tm *timeinfo) {
  static const char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  static const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

  _display->setFont(f96);

  _display->fillRect(0, 0, 400, 219, GxEPD_WHITE);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(GxEPD_BLACK, GxEPD_WHITE);

  _display->setCursor(0, 140);
  _display->printf("%02d%02d", timeinfo->tm_hour, timeinfo->tm_min);

  _display->setFont(f18);
  _display->setCursor(400-48, 140);
  _display->printf("%02d", timeinfo->tm_sec);

  _display->setFont(f18);
  _display->setCursor(0, 210-24);
  _display->printf("%s %s %d %d", days[timeinfo->tm_wday], months[timeinfo->tm_mon], timeinfo->tm_mday, 1900 + timeinfo->tm_year);

  int16_t x, y;
  uint16_t w, h;

  _display->getTextBounds("16:17", 0, 0, &x, &y, &w, &h);
  Serial.printf("x %d, y %d, w %d, h %d\n", x, y, w, h);

  _display->updateWindow(0, 0, 400, 199);
}

void ClockUI::clear_time() {
  _display->fillRect(10, 10, 200, 200, GxEPD_WHITE);
  _display->updateWindow(10, 10, 200, 200);
}

void ClockUI::show_indoor(float temperature, float humidity) {
  temperature = (temperature * 9 / 5) + 32 - 3.2;

  _display->setFont(f24);

  _display->fillRect(0, 224, 200, 300, GxEPD_WHITE);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(GxEPD_BLACK, GxEPD_WHITE);

  _display->setCursor(0, 270);
  _display->printf("%d %d%%\n", (int)temperature, (int)humidity);
  _display->updateWindow(0, 224, 200, 300);
}

void ClockUI::show_outdoor(float temperature, float humidity) {
  temperature = (temperature * 9 / 5) + 32 - 3.2;

  _display->setFont(f24);

  _display->fillRect(200, 224, 400, 300, GxEPD_WHITE);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(GxEPD_BLACK, GxEPD_WHITE);

  _display->setCursor(200, 270);
  _display->printf("%d %d%%", (int)temperature, (int)humidity);

  _display->updateWindow(200, 224, 400, 300);
}


void ClockUI::test() {
  //  _display->setFont(f);

  _display->fillScreen(GxEPD_WHITE);
  _display->update();
  Serial.println("1");

  _display->setTextColor(GxEPD_BLACK);
  _display->setTextSize(8);

  int16_t x, y;
  uint16_t w, h;

  _display->getTextBounds("16:17:05", 0, 0, &x, &y, &w, &h);
  Serial.printf("x %d, y %d, w %d, h %d\n", x, y, w, h);

  _display->println("16:17:05");
  _display->update();

  delay(8000);

  _display->println("Hello World!");

  _display->setTextSize(1);
  _display->println(1234.56);
  //  _display->updateWindow(0, 0, 200, 200);
  _display->update();
  Serial.println("2");
  delay(2500);

  _display->setTextSize(3);
  _display->println(0xDEADBEEF, HEX);
  _display->println();
  _display->update();
  Serial.println("3");
  delay(2500);


  _display->setTextSize(5);
  _display->println("Groop");
  _display->setTextSize(2);
  _display->println("I implore thee,");
  _display->setTextSize(1);
  _display->update();
  Serial.println("4");
  delay(2500);

  _display->println("my foonting turlingdromes.");
  _display->println("And hooptiously drangle me");
  _display->println("with crinkly bindlewurdles,");
  _display->println("Or I will rend thee");
  _display->println("in the gobberwarts");
  _display->println("with my blurglecruncheon,");
  _display->println("see if I don't!");
  _display->update();
  Serial.println("5");
  delay(2500);
}

