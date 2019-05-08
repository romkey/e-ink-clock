#!/bin/sh
export FILES="cloudy cloudy-gusts day-cloudy-gusts day-fog day-hail day-haze day-lightning day-rain day-rain-mix day-showers day-sleet day-snow day-sprinkle day-storm-showers day-sunny day-sunny-overcast day-thunderstorm dust fog hail hot hurricane lightning night-alt-cloudy night-alt-cloudy-gusts night-alt-hail night-alt-lightning night-alt-rain night-alt-rain-mix night-alt-showers night-alt-sleet night-alt-snow night-alt-sprinkle night-alt-storm-showers night-alt-thunderstorm night-clear night-fog rain rain-mix showers sleet smoke snow snowflake-cold sprinkle storm-showers strong-wind thunderstorm tornado windy"

for FILE in $FILES
do
  echo $FILE
  ln -s ../../../weather-icons/svg/wi-$FILE.svg .
done
