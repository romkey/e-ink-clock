#include "ui.h"

#include "wifi_bmp.h"

// screen is 300h x 400w

void ClockUI::show_wifi_symbol() {
  _display->drawBitmap(wifi_symbol, 69, 19, 262, 193, GxEPD_BLACK);
}

void ClockUI::clear_wifi_symbol() {
  //  _display->fillScreen(GxEPD_WHITE);
  _display->fillRect(69, 19, 262, 193, GxEPD_WHITE);
  _display->updateWindow(69, 19, 262, 193);
}

void ClockUI::show_network_info(IPAddress local_ip, const char* name, const char* ssid) {
  static const GFXfont* f = &FreeMonoBold12pt7b;

  _display->setFont(f);

  _display->fillRect(10, 10, 200, 200, GxEPD_WHITE);
  _display->setCursor(10, 10);

  _display->printf("Connected to: %s\n", ssid);

  _display->print("IP: ");
  _display->println(local_ip);
  
  _display->printf("Hostname: %s.local", name);

  _display->updateWindow(10, 10, 200, 200);
}

void ClockUI::clear_network_info() {
  _display->fillRect(10, 10, 200, 200, GxEPD_WHITE);
  _display->updateWindow(10, 10, 200, 200);
}

void ClockUI::show_time() {
  static const GFXfont* f = &FreeMonoBold24pt7b;

  _display->setFont(f);

  _display->fillRect(10, 10, 200, 200, GxEPD_WHITE);
  _display->setCursor(10, 10);

  _display->updateWindow(10, 10, 200, 200);
}

void ClockUI::clear_time() {
  _display->fillRect(10, 10, 200, 200, GxEPD_WHITE);
  _display->updateWindow(10, 10, 200, 200);
}


