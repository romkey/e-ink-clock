# e-ink wifi clock

![Test build](https://github.com/romkey/e-ink-clock/actions/workflows/main.yml/badge.svg)

_This is a work in progress. I'm still testing and updating the hardware, and the software is incomplete. Build at your own risk._

Use a Waveshare epaper (e-ink) display as a digital wifi clock. The clock synchronizes its time with NTP and can show local weather using OpenWeatherMap.

The clock also supports an optional environmental sensor module, which allows it to report the temperature, humidity and air pressure in the space it's in.

It also supports an optional DS3231 I2C real time clock with battery backup module. This allows it to know the time immediately upon booting, even without NTP, and to keep time more accurately than just the ESP32 is capable of.

FInally, it also supports an optional ambient light sensor. This allows the clock to power down the e-ink module in the dark (when it is unreadable), which should extend the lifespan of the module.


# Hardware

Eagle schematics and board are in the `eagle` directory. 

_The hardware is still in testing; I cannot make any promises about its suitability for any use or its reliability._

Prices are obviously from the time I made this list. Links to parts are meant to show parts that have identical pinouts to the ones I used in the PC board layout. The specific items may become unavailable over time or the listings might even change to other parts with incompatible pinouts, so *please be careful to make sure you get parts with the correct pinouts* if you want to use the PC board - this is meant to be a guide to the parts and not a specific shopping list. 

| Part                               | AliExpress | Amazon | Notes |
|-------------------|--------:|-------:|--|
|Waveshare 4.2" epaper module | [$28.79](https://www.aliexpress.com/item/400x300-4-2inch-E-Ink-display-module-No-backlight-Ultra-low-power-consumption-SPI-interface-Compatible/32826601765.html) | [$38.99](https://www.amazon.com/gp/product/B074NR1SW2) | |
|Wemos LOLIN32 ESP32 | [$5.50](https://www.aliexpress.com/item/ESP-32-ESP32-ESP-WROOM-32-for-WeMos-D1-LOLIN32-V1-0-02-WIFI-Bluetooth-Development/32887251214.html) | [$9.99 + $4.99](https://www.amazon.com/Beaster-LOLIN32-V1-0-0-Bluetooth-Development/dp/B07KFF1GJK) | |
|BME280 breakout | [$2.50](https://www.aliexpress.com/item/High-Accuracy-3In1-BME280-Digital-Sensor-Temperature-Humidity-Barometric-Pressure-Sensor-Module-GY-BME280-I2C-SPI/32817230797.html) | [$11.20](https://www.amazon.com/gp/product/B0775XFW69) | *optional*  lets you read the temperature, hunmidity and pressure where the clock is |
|DS3231 RTC breakout | [$1.04](https://www.aliexpress.com/item/DS3231-AT24C32-IIC-High-Precision-RTC-Module-Clock-Timer-Memory-Module/2037934408.html) | [$6.99](https://www.amazon.com/gp/product/B01IXXACD0) | *optional* keeps more accurate time, works without NTP or wifi |
|photoresistor | []() | []() | *optional* lets the clock power off display in the dark |
|         |$37.83 | $72.16 |

Obviously AliExpress is an economical choice if you can wait a month or more for your parts to arrive. 

The printed circuit board uses breakout boards and is easy to assemble. The current version is at https://oshpark.com/shared_projects/apAIFWjg



# Software

The e-ink clock software is built using the Arduino SDK. It builds under [PlatformIO](https://platformio.org/) but with a little rearranging you could also build it with the Arduino IDE.

The application uses the [GxEPD](https://github.com/ZinggJM/GxEPD) library to drive the Waveshare display. GxEPD is a subclass of [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library), so you can use its API to talk to the display. It also supports fast partial update on some Waveshare displays, which lets you write to the display much faster than a full screen write, without distracting flashing.

## Resources:

- turn an image file into a C language bitmap - [image2cpp](https://javl.github.io/image2cpp/)
- use `fontconvert` from Adafruit GFX Library to convert fonts to Adafruit GFX format


## Configuration

Copy `src/config.h-example` to `src/config.h` and set the constants in the file as you need them:
- `WIFI_SSID1`, `WIFI_PASSWORD1` - your primary WiFi SSID and password 
- `WIFI_SSID2`, `WIFI_PASSWORD2` - your backup WiFi SSID and password  - set to the same as primary if you don't have a backup
- `WIFI_SSID3`, `WIFI_PASSWORD3` - your bakup backup WiFi SSID and password  - set to the same as backup if you don't have a backup backup

## License

Permission to use this software is granted under the [MIT License](https://romkey.mit-license.org).
