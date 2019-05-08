#pragma once

#include "JsonListener.h"
#include "OpenWeatherMap.h"

class OpenWeatherMapCurrentListener : public JsonListener {
  friend class OpenWeatherMap;

public:
  virtual void startDocument() {};
  virtual void startArray() {};
  virtual void startObject() {};
  virtual void whitespace(char c) {};
  virtual void key(String key);
  virtual void value(String value);
  virtual void endObject() {};
  virtual void endArray() {};
  virtual void endDocument() {};

private:
  enum { OWMCL_STATE_NULL, OWMCL_STATE_TEMP, OWMCL_STATE_TEMP_MIN, OWMCL_STATE_TEMP_MAX, OWMCL_STATE_HUMIDITY, OWMCL_STATE_PRESSURE,
	 OWMCL_STATE_SUNRISE, OWMCL_STATE_SUNSET, OWMCL_STATE_WIND_DEG, OWMCL_STATE_WIND_SPEED, OWMCL_STATE_RAIN1H, OWMCL_STATE_WEATHER_ID
  } _state;

  OpenWeatherMapConditions _conditions;
};
