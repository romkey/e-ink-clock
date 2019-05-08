#pragma once

#define OPENWEATHERMAP_API_KEY_LENGTH 33

#define _OPENWEATHERMAP_FINGERPRINT  "6C:9D:1E:27:F1:13:7B:C7:B6:15:90:13:F2:D0:29:97:A4:5B:3F:7E"

#define KELVIN_TO_C(k) (k - 273.15)
#define C_TO_F(c) (c * 9 / 5.0) + 32

class OpenWeatherMapConditions {
public:
  float temp = 0;
  float temp_min = 0;
  float temp_max = 0;

  uint8_t humidity = 0;
  uint16_t pressure = 0;

  uint16_t wind_deg = 0;
  float wind_speed = 0;

  float rain1h = 0;

  uint16_t weather_id = 0;

  uint32_t sunrise = 0;
  uint32_t sunset = 0;
};

class OpenWeatherMap {
 public:
  OpenWeatherMap(const char* api_key, const char* location);
  OpenWeatherMap(const char* api_key, int city_code);
  OpenWeatherMap(const char* api_key);

  int update_current();
  OpenWeatherMapConditions current() { return _current_conditions; };

  const char* last_error() { return _last_error; };

 private:
  char _api_key[OPENWEATHERMAP_API_KEY_LENGTH + 1];
  const char* _zip_code = "";
  int _city_id = 0;

  unsigned long _last_updated;
  char _last_error[32] = "";

  OpenWeatherMapConditions _current_conditions;
  OpenWeatherMapConditions _5day_forecast[5];

  const char* _openweathermap_fingerprint;
};
