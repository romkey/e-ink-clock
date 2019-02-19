# e-ink wifi clock

Use a Waveshare epaper (e-ink) display as a digital wifi clock. The clock synchronizes its time with NTP and can show local weather using OpenWeatherMap.

The clock also supports an optional environmental sensor module, which allows it to report the temperature, humidity and air pressure in the space it's in.

It also supports an optional DS3231 I2C real time clock with battery backup module. This allows it to know the time immediately upon booting, even without NTP, and to keep time more accurately than just the ESP32 is capable of.

FInally, it also supports an optional ambient light sensor. This allows the clock to power down the e-ink module in the dark (when it is unreadable), which should extend the lifespan of the module.


# Hardware

- Wemos LOLIN32 ESP32
- Waveshare epaper 4.2inch module
- optional BME280 I2C temperature/humidity/pressure sensor
- optional I2C real time clock

# Software

The e-ink clock software is built using the Arduino SDK. It builds under [PlatformIO](https://platformio.org/) but with a little rearranging you could also build it with the Arduino IDE.

## Resources:

- turn an image file into a C language bitmap - [image2cpp](https://javl.github.io/image2cpp/)

