// #include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include "OpenWeatherMap.h"

//------------------------------------------ Universal weather request parcer for opewWeatherMap site ---------
void OWMrequest::doUpdate(String url, byte maxForecasts) {
  JsonStreamingParser parser;
  parser.setListener(this);

  HTTPClient http;

  Serial.print("OWMrequest ");
  Serial.println(url);

  http.begin(url);
  bool isBody = false;
  char c;
  int size;
  int httpCode = http.GET();

  if(httpCode > 0) {
    Serial.printf("GOT MILK %d\n");

    String result = http.getString();
    Serial.println(result);

    for(int i = 0; i < result.length(); i++) {
      parser.parse(result.charAt(i));
    }

#if 0
    WiFiClient * client = http.getStreamPtr();
    while(client->connected()) {
      while((size = client->available()) > 0) {
        c = client->read();
        if (c == '{' || c == '[') {
          isBody = true;
        }
        if (isBody) {
          parser.parse(c);
        }

	Serial.print(c);
      }
    }
#endif

  } else {
    Serial.printf("HTTP fail %d\n", httpCode);
  }
}

void OWMrequest::init(void) {
  for (byte i = 0; i < OWM_max_layers; ++i)
    p_key[i] = "";
  currentParent = currentKey = "";
}

void OWMrequest::startObject() {
  for (byte i = OWM_max_layers-1; i > 0; --i) {
    p_key[i] = p_key[i-1];
  }
  p_key[0] = currentParent;
  currentParent = currentKey;
}

void OWMrequest::endObject() {
  currentParent = p_key[0];
  for (byte i = 0; i < OWM_max_layers-1; ++i) {
    p_key[i] = p_key[i+1];
  }
  currentKey = "";
}

//------------------------------------------ Current weather conditions from openweatrhermap.org --------------
void OWMconditions::updateConditions(OWM_conditions *conditions, String apiKey, String country, String city, String units, String language) {
  this->conditions = conditions;
  OWMrequest::init();

  units = "Imperial";

  String url = "http://api.openweathermap.org/data/2.5/weather?zip=97217";
  if (language != "") url += "&lang="  + language;
  if (units != "")    url += "&units=" + units;
  url +=  + "&appid=" + apiKey;
  doUpdate(url);
  this->conditions = nullptr;
}

void OWMconditions::value(String value) {
  if (currentKey == "lon") {
    conditions->longtitude = value;
  } else
  if (currentKey == "lat") {
    conditions->latitude = value;
  } else
  if (currentKey == "id") {
    conditions->id = value;
  } else
  if (currentKey == "main") {
    conditions->main = value;
  } else
  if (currentKey == "description") {
    conditions->description = value;
  } else
  if (currentKey == "icon") {
    conditions->icon = value;
  } else
  if (currentKey == "temp") {
    conditions->temp = value;
  } else
  if (currentKey == "pressure") {
    conditions->pressure = value;
  } else
  if (currentKey == "humidity") {
    conditions->humidity = value;
  } else
  if (currentKey == "temp_min") {
    conditions->t_min = value;
  } else
  if (currentKey == "temp_max") {
    conditions->t_max = value;
  } else
  if (currentKey == "visibility") {
    conditions->visibility = value;
  } else
  if (currentParent == "wind" && currentKey == "speed") {
    conditions->w_speed = value;
  } else
  if (currentParent == "wind" && currentKey == "deg") {
    conditions->w_deg = value;
  } else
  if (currentParent == "clouds" && currentKey == "all") {
    conditions->cond = currentParent;
    conditions->cond_value = value;
  } else
  if (currentParent == "rain" && currentKey == "3h") {
    conditions->cond = currentParent;
    conditions->cond_value = value;
  }  else
  if (currentParent == "snow" && currentKey == "3h") {
    conditions->cond = currentParent;
    conditions->cond_value = value;
  } else
  if (currentKey == "dt") {
    conditions->dt = value;
  } else
  if (currentKey == "sunrise") {
    conditions->sunrise = value;
  } else
  if (currentKey == "sunset") {
    conditions->sunset = value;
  }
}

//------------------------------------------ Five day forecast from openweatrhermap.org -----------------------
byte OWMfiveForecast::updateForecast(OWM_fiveForecast *forecasts, byte maxForecasts, String apiKey, String country, String city, String units, String language) {
  this->forecasts = forecasts;
  this->max_forecasts = maxForecasts;
  OWMrequest::init();
  index = 0;
  timestamp = 0;

  String url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + country;
  if (language != "") url += "&lang="  + language;
  if (units != "")    url += "&units=" + units;
  url +=  + "&appid=" + apiKey;
  doUpdate(url, maxForecasts);

  this->forecasts = nullptr;
  return index;
}

void OWMfiveForecast::value(String value) {
  if (currentKey == "dt") {
    if (timestamp == 0) {
      index = 0;
      forecasts[index].dt = value;
      timestamp = value.toInt();
    } else {
      uint32_t t = value.toInt();
      if (t > timestamp) {                          // new forecast time
	      if (index < max_forecasts - 1) {
    	    ++index;
          timestamp = t;
    	    forecasts[index].dt = value;
    	  }
      }
    }
  } else
  if (currentKey == "temp") {
    forecasts[index].temp = value;
  } else
  if (currentKey == "temp_min") {
    forecasts[index].t_min = value;
  } else
  if (currentKey == "temp_max") {
    forecasts[index].t_max = value;
  } else
  if (currentKey == "pressure") {
    forecasts[index].pressure = value;
  } else
  if (currentKey == "sea_level") {
    forecasts[index].sea_pressure = value;
  } else
  if (currentKey == "humidity") {
    forecasts[index].humidity = value;
  } else
  if (currentParent == "weather" && currentKey == "description") {
    forecasts[index].description = value;
  } else
  if (currentParent == "weather" && currentKey == "icon") {
    forecasts[index].icon = value;
  } else
  if (currentParent == "weather" && currentKey == "id") {
    forecasts[index].id = value;
  } else
  if (currentParent == "wind" && currentKey == "speed") {
    forecasts[index].w_speed = value;
  } else
  if (currentParent == "wind" && currentKey == "deg") {
    forecasts[index].w_deg = value;
  } else
  if (currentParent == "clouds" && currentKey == "all") {
    forecasts[index].cond = currentParent;
    forecasts[index].cond_value = value;
  } else
  if (currentParent == "rain" && currentKey == "3h") {
    forecasts[index].cond = currentParent;
    forecasts[index].cond_value = value;
  }  else
  if (currentParent == "snow" && currentKey == "3h") {
    forecasts[index].cond = currentParent;
    forecasts[index].cond_value = value;
  }
}

//------------------------------------------ Sexteen days weather forecast from openweathermap.org -----------
byte OWMsixteenForecast::updateForecast(OWM_sixteenLocation *location, OWM_sixteenForecast *forecasts,
                                        byte maxForecasts, String apiKey, String country, String city, String units, String language) {
  this->location  = location;
  this->forecasts = forecasts;
  this->max_forecasts = maxForecasts;
  OWMrequest::init();
  index = 0;
  timestamp = 0;

  String url = "http://api.openweathermap.org/data/2.5/forecast/daily?q=" + city + "," + country;
  if (language != "") url += "&lang="  + language;
  if (units != "")    url += "&units=" + units;
  url +=  + "&appid=" + apiKey;    
  doUpdate(url, maxForecasts);
  this->forecasts = nullptr;
  this->location  = nullptr;
  return index;
}

void OWMsixteenForecast::value(String value) {
  // First fill up the location info
  if (location) {
    if (currentParent == "city" && currentKey == "id") {
      location->city_id = value;
    } else
    if (currentParent == "city" && currentKey == "name") {
      location->city_name = value;
    } else
    if (currentParent == "coord" && currentKey == "lon") {
      location->longtitude = value;
    } else
    if (currentParent == "coord" && currentKey == "lat") {
      location->latitude = value;
    } else
    if (currentParent == "city" && currentKey == "country") {
      location->country = value;
    } else
    if (currentParent == "city" && currentKey == "population") {
      location->population = value;
    } else
    if (currentKey == "cod") {
      location->cod = value;
    } else
    if (currentKey == "message") {
      location->message = value;
    }
  }

  // Second, fill up the dayly forecast
  if (currentKey == "dt") {
    if (timestamp == 0) {
      index = 0;
      forecasts[index].dt = value;
      timestamp = value.toInt();
    } else {
      uint32_t t = value.toInt();
      if (t > timestamp) {                          // new forecast time
        if (index < max_forecasts - 1) {
          ++index;
          timestamp = t;
          forecasts[index].dt = value;
        }
      }
    }
  } else
  if (currentParent == "temp" && currentKey == "min") {
    forecasts[index].t_min = value;
  } else
  if (currentParent == "temp" && currentKey == "max") {
    forecasts[index].t_max = value;
  } else
  if (currentParent == "temp" && currentKey == "night") {
    forecasts[index].t_night = value;
  } else
  if (currentParent == "temp" && currentKey == "morn") {
    forecasts[index].t_morning = value;
  } else
  if (currentParent == "temp" && currentKey == "day") {
    forecasts[index].t_day = value;
  } else
  if (currentParent == "temp" && currentKey == "eve") {
    forecasts[index].t_evening = value;
  } else

  if (currentKey == "pressure") {
    forecasts[index].pressure = value;
  } else
  if (currentKey == "humidity") {
    forecasts[index].humidity = value;
  } else
  if (currentParent == "weather" && currentKey == "description") {
    forecasts[index].description = value;
  } else
  if (currentParent == "weather" && currentKey == "icon") {
    forecasts[index].icon = value;
  } else
  if (currentParent == "weather" && currentKey == "id") {
    forecasts[index].id = value;
  } else
  if (currentParent == "weather" && currentKey == "description") {
    forecasts[index].description = value;
  } else
  
  if (currentKey == "speed") {
    forecasts[index].w_speed = value;
  } else
  if (currentKey == "deg") {
    forecasts[index].w_deg = value;
  } else
  if (currentKey == "clouds") {
    forecasts[index].clouds = value;
  }
}
