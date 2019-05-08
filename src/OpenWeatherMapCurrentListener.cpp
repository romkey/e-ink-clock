#include "OpenWeatherMapCurrentListener.h"

void OpenWeatherMapCurrentListener::key(String key) {
  if(key == "temp") {
    _state = OWMCL_STATE_TEMP;
    return;
  }

  if(key == "temp_min") {
    _state = OWMCL_STATE_TEMP_MIN;
    return;
  }
  
  if(key == "temp_max") {
    _state = OWMCL_STATE_TEMP_MAX;
    return;
  }

  if(key == "humidity") {
    _state = OWMCL_STATE_HUMIDITY;
    return;
  }

  if(key == "sunrise") {
    _state = OWMCL_STATE_SUNRISE;
    return;
  }

  if(key == "sunset") {
    _state = OWMCL_STATE_SUNSET;
    return;
  }

  if(key == "wind_deg") {
    _state = OWMCL_STATE_WIND_DEG;
    return;
  }

  if(key == "wind_speed") {
    _state = OWMCL_STATE_WIND_SPEED;
    return;
  }

  if(key == "rain1h") {
    _state = OWMCL_STATE_RAIN1H;
    return;
  }

  // THIS NOT RIGHT
  if(key == "id") {
    _state = OWMCL_STATE_WEATHER_ID;
    return;
  }

}

void OpenWeatherMapCurrentListener::value(String value) {
  switch(_state) {
  case OWMCL_STATE_NULL:
    return;

  case OWMCL_STATE_TEMP:
    _conditions.temp = KELVIN_TO_C(value.toFloat());
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_TEMP_MIN:
    _conditions.temp_min = KELVIN_TO_C(value.toFloat());
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_TEMP_MAX:
    _conditions.temp_max = KELVIN_TO_C(value.toFloat());
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_PRESSURE:
    _conditions.pressure = value.toInt();
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_HUMIDITY:
    _conditions.humidity = value.toInt();
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_SUNRISE:
    _conditions.sunrise = atol(value.c_str());
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_SUNSET:
    _conditions.sunset = atol(value.c_str());
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_WIND_DEG:
    _conditions.wind_deg = value.toInt();
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_WIND_SPEED:
    _conditions.wind_speed = value.toFloat();
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_RAIN1H:
    _conditions.rain1h = value.toFloat();
    _state = OWMCL_STATE_NULL;
    return;

  case OWMCL_STATE_WEATHER_ID:
    _conditions.weather_id = value.toInt();
    _state = OWMCL_STATE_NULL;
    return;
  }
}
