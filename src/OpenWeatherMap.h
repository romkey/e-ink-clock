#ifndef OpenWeather_h
#define OpenWeather_h

#include <JsonListener.h>
#include <JsonStreamingParser.h>

//------------------------------------------ Universal weather request parcer for opewWeatherMap site ---------
#define OWM_max_layers 4
class OWMrequest: public JsonListener {
  public:
    void init(void);
    OWMrequest() : JsonListener()                   { }
    virtual void key(String key)                    { currentKey = String(key); }
    virtual void endObject();
    virtual void startObject();
    virtual void whitespace(char c)                 { }
    virtual void startDocument()                    { }
    virtual void endArray()                         { }
    virtual void endDocument()                      { }
    virtual void startArray()                       { }
  protected:
    void   doUpdate(String url, byte maxForecasts = 0);
    String currentKey;
    String currentParent;
    String p_key[OWM_max_layers];
};

//------------------------------------------ Current weather conditions from openweatrhermap.org --------------
typedef struct sOWM_conditions {
  String longtitude;
  String latitude;
  String id;
  String main;
  String description;
  String icon;
  String temp;
  String pressure;
  String humidity;
  String t_min;
  String t_max;
  String visibility;
  String w_speed;
  String w_deg;
  String cond;                                      // conditions: cloud, rain, snow
  String cond_value;
  String dt;
  String sunrise;
  String sunset;
} OWM_conditions;

class OWMconditions : public OWMrequest {
  public:
    OWMconditions()                                { currentParent = ""; }
    void    init(void)                             { currentParent = ""; }
    void    updateConditions(OWM_conditions *conditions, String apiKey,
              String country, String city, String units = "", String language = "");
    virtual void value(String value);

  private:
    OWM_conditions *conditions;
};


//------------------------------------------ Five day forecast from openweatrhermap.org -----------------------
typedef struct sOWM_fiveForecast {
  String dt;
  String temp;
  String t_min;
  String t_max;
  String pressure;
  String sea_pressure;
  String humidity;
  String id;
  String main;
  String description;
  String icon;
  String w_speed;
  String w_deg;
  String cond;                                      // conditions: cloud, rain, snow
  String cond_value;
} OWM_fiveForecast;

class OWMfiveForecast : public OWMrequest {
  public:
    OWMfiveForecast()                               { }
    byte    updateForecast(OWM_fiveForecast *forecasts, byte maxForecasts, String apiKey,
              String country, String city, String units = "", String language = "");
    virtual void value(String value);

  private:
    byte     index;
    uint32_t timestamp;
    byte     max_forecasts;
    OWM_fiveForecast *forecasts;
};

//------------------------------------------ Sexteen days weather forecast from openweathermap.org -----------
typedef struct sOWM_sixteenLocation {
  String city_id;
  String city_name;
  String longtitude;
  String latitude;
  String country;
  String population;
  String cod;
  String message;  
} OWM_sixteenLocation;

typedef struct sOWM_sixteenForecast {
  String dt;
  String t_min;
  String t_max;
  String t_night;
  String t_morning;
  String t_day;
  String t_evening;
  String pressure;
  String humidity;
  String id;
  String main;
  String description;
  String icon;
  String w_speed;
  String w_deg;
  String clouds;
} OWM_sixteenForecast;

class OWMsixteenForecast : public OWMrequest {
  public:
    OWMsixteenForecast()                            { }
    byte    updateForecast(OWM_sixteenLocation *location, OWM_sixteenForecast *forecasts, byte maxForecasts, String apiKey,
              String country, String city, String units = "", String language = "");
    virtual void value(String value);

  private:
    byte     index;
    uint32_t timestamp;
    byte     max_forecasts;
    OWM_sixteenLocation *location;
    OWM_sixteenForecast *forecasts;
};

#endif
