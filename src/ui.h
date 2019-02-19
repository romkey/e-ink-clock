#pragma once

#include <Arduino.h>

#ifdef U8G2
#include <U8g2_for_Adafruit_GFX.h>
U8G2_FOR_ADAFRUIT_GFX u8g2;
#endif

#include <GxEPD.h>
#include <GxEPD.h>
#include <GxGDEW042T2/GxGDEW042T2.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

class ClockUI {
public:
  ClockUI(GxEPD_Class *display) : _display(display) { };

  void begin();

  void test();

  void show_wifi_symbol();
  void clear_wifi_symbol();

  void show_network_info(IPAddress local_ip, const char* name, const char* ssid);
  void clear_network_info();

  void setup_time();
  void show_time(struct tm* timeinfo);
  void clear_time();

private:
  GxEPD_Class* _display;
};

