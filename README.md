# e-ink wifi clock

Use a Waveshare epaper (e-ink) display as a digital wifi clock. The clock synchronizes its time with NTP and can show local weather using OpenWeatherMap.

The clock also supports an optional environmental sensor module, which allows it to report the temperature, humidity and air pressure in the space it's in.

It also supports an optional DS3231 I2C real time clock with battery backup module. This allows it to know the time immediately upon booting, even without NTP, and to keep time more accurately than just the ESP32 is capable of.

FInally, it also supports an optional ambient light sensor. This allows the clock to power down the e-ink module in the dark (when it is unreadable), which should extend the lifespan of the module.


# Hardware

Prices are obviously from the time I made this list. Links to parts are meant to show parts that have identical pinouts to the ones I used in the PC board layout. The specific items may become unavailable over time or the listings might even change to other parts with incompatible pinouts, so *please be careful to make sure you get parts with the correct pinouts* if you want to use the PC board - this is meant to be a guide to the parts and not a specific shopping list. 

| Part                               | AliExpress | Amazon | Notes |
|-------------------|--------:|-------:|--|
|Waveshare 4.2" epaper module | [$28.79](https://www.aliexpress.com/item/400x300-4-2inch-E-Ink-display-module-No-backlight-Ultra-low-power-consumption-SPI-interface-Compatible/32826601765.html) | [$38.99](https://www.amazon.com/gp/product/B074NR1SW2) | |
|Wemos LOLIN32 ESP32 | []() | [$9.99 + $4.99](https://www.amazon.com/Beaster-LOLIN32-V1-0-0-Bluetooth-Development/dp/B07KFF1GJK) | |
|BME280 breakout | []() | [$11.20](https://www.amazon.com/gp/product/B0775XFW69) | *optional*  lets you read the temperature, hunmidity and pressure where the clock is |
|DS3231 RTC breakout | []() | [$6.99](https://www.amazon.com/gp/product/B01IXXACD0) | *optional* keeps more accurate time, works without NTP or wifi |
|photoresistor | []() | []() | *optional* lets the clock power off display in the dark |
|         | | $72.16 |



# Software

The e-ink clock software is built using the Arduino SDK. It builds under [PlatformIO](https://platformio.org/) but with a little rearranging you could also build it with the Arduino IDE.

## Resources:

- turn an image file into a C language bitmap - [image2cpp](https://javl.github.io/image2cpp/)

