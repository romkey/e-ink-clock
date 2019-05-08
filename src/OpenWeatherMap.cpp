#ifndef ESP32
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <JsonStreamingParser.h>

#include "OpenWeatherMap.h"
#include "OpenWeatherMapCurrentListener.h"

#ifdef OPENWEATHERMAP_HTTPS
#define OPENWEATHERMAP_PROTOCOL "https"
#else
#define OPENWEATHERMAP_PROTOCOL "http"
#endif

#define WEATHER_URL        OPENWEATHERMAP_PROTOCOL "://api.openweathermap.org/data/2.5/weather"
#define FORECAST_5DAY_URL  OPENWEATHERMAP_PROTOCOL "://api.openweathermap.org/data/2.5/forecast"

#define PORTLAND_CITY_ID    5746545

#ifdef ESP32
const char* _openweathermap_root_certificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIE0DCCA7igAwIBAgIBBzANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx" \
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT" \
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp" \
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTExMDUwMzA3MDAwMFoXDTMxMDUwMzA3" \
"MDAwMFowgbQxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH" \
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjEtMCsGA1UE" \
"CxMkaHR0cDovL2NlcnRzLmdvZGFkZHkuY29tL3JlcG9zaXRvcnkvMTMwMQYDVQQD" \
"EypHbyBEYWRkeSBTZWN1cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwggEi" \
"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC54MsQ1K92vdSTYuswZLiBCGzD" \
"BNliF44v/z5lz4/OYuY8UhzaFkVLVat4a2ODYpDOD2lsmcgaFItMzEUz6ojcnqOv" \
"K/6AYZ15V8TPLvQ/MDxdR/yaFrzDN5ZBUY4RS1T4KL7QjL7wMDge87Am+GZHY23e" \
"cSZHjzhHU9FGHbTj3ADqRay9vHHZqm8A29vNMDp5T19MR/gd71vCxJ1gO7GyQ5HY" \
"pDNO6rPWJ0+tJYqlxvTV0KaudAVkV4i1RFXULSo6Pvi4vekyCgKUZMQWOlDxSq7n" \
"eTOvDCAHf+jfBDnCaQJsY1L6d8EbyHSHyLmTGFBUNUtpTrw700kuH9zB0lL7AgMB" \
"AAGjggEaMIIBFjAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjAdBgNV" \
"HQ4EFgQUQMK9J47MNIMwojPX+2yz8LQsgM4wHwYDVR0jBBgwFoAUOpqFBxBnKLbv" \
"9r0FQW4gwZTaD94wNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8v" \
"b2NzcC5nb2RhZGR5LmNvbS8wNQYDVR0fBC4wLDAqoCigJoYkaHR0cDovL2NybC5n" \
"b2RhZGR5LmNvbS9nZHJvb3QtZzIuY3JsMEYGA1UdIAQ/MD0wOwYEVR0gADAzMDEG" \
"CCsGAQUFBwIBFiVodHRwczovL2NlcnRzLmdvZGFkZHkuY29tL3JlcG9zaXRvcnkv" \
"MA0GCSqGSIb3DQEBCwUAA4IBAQAIfmyTEMg4uJapkEv/oV9PBO9sPpyIBslQj6Zz" \
"91cxG7685C/b+LrTW+C05+Z5Yg4MotdqY3MxtfWoSKQ7CC2iXZDXtHwlTxFWMMS2" \
"RJ17LJ3lXubvDGGqv+QqG+6EnriDfcFDzkSnE3ANkR/0yBOtg2DZ2HKocyQetawi" \
"DsoXiWJYRBuriSUBAA/NxBti21G00w9RKpv0vHP8ds42pM3Z2Czqrpv1KrKQ0U11" \
"GIo/ikGQI31bS/6kA1ibRrLDYGCD+H1QQc7CoZDDu+8CL9IVVO5EFdkKrqeKM+2x" \
"LXY2JtwE65/3YR8V3Idv7kaWKK2hJn0KCacuBKONvPi8BDAB" \
"-----END CERTIFICATE-----\n";
#endif

OpenWeatherMap::OpenWeatherMap(const char* api_key, const char* zip) : OpenWeatherMap::OpenWeatherMap(api_key) {
  _zip_code = zip;
}

OpenWeatherMap::OpenWeatherMap(const char* api_key, int id) : OpenWeatherMap::OpenWeatherMap(api_key) {
  _city_id = id;
}

OpenWeatherMap::OpenWeatherMap(const char* api_key) {
  strncpy(_api_key, api_key, OPENWEATHERMAP_API_KEY_LENGTH);
  _openweathermap_fingerprint = _OPENWEATHERMAP_FINGERPRINT;
}

int OpenWeatherMap::update_current() {
  HTTPClient http;
  char url[250];

  snprintf(url, 250, "%s?id=%d&APPID=%s", WEATHER_URL, _city_id, _api_key);
  //  snprintf(url, 250, "%s?id=%d&APPID=%s", FORECAST_5DAY_URL, _city_id, _api_key);
  Serial.println(url);

#ifdef ESP32
  // certificate: openssl s_client -showcerts -connect maker.ifttt.com:443 < /dev/null
  //  http.begin(url, _openweathermap_root_certificate);
  http.begin(url);
#else
  // fingerprint: openssl s_client -connect maker.ifttt.com:443  < /dev/null 2>/dev/null | openssl x509 -fingerprint -noout | cut -d'=' -f2
  //  http.begin(url, _openweathermap_fingerprint);
  http.begin(url);
#endif

  Serial.println("1");

  int httpCode = http.GET();
  Serial.println("1.0");

  if(httpCode > 0) {
    Serial.println("1.1");

    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if(httpCode != HTTP_CODE_OK) {
      Serial.println("1.2");
      http.end();
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      return -1;
    }
  } else {
    Serial.println(httpCode);
    Serial.println("1.3");
    http.end();
    Serial.println("1.4");
    return -1;
  }

  Serial.println("2");

  Serial.println("JSON!");
  String json_str = http.getString();
  Serial.println(json_str);
 
  JsonStreamingParser parser;
  OpenWeatherMapCurrentListener listener;

  parser.setListener(&listener);

  for (int i = 0; i < json_str.length(); i++) {
    parser.parse(json_str[i]);
  }

  Serial.println(C_TO_F(listener._conditions.temp));

  _current_conditions = listener._conditions;

  return 0;
}
