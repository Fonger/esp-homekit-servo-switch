# esp-homekit-servo-switch

This is a HomeKit NodeMCU servo controller for my room light switch

## Home Appliance

1. Use servo to toggle the light switch on wall physically.
2. A button to turn on/off the light switch.
2. Read air quality data including AQI, PM2.5, PM10, NO<sub>2</sub>, SO<sub>2</sub>, CO and O<sub>3</sub> data from [LASS(Location Aware Sensing System)](https://lass-net.org/) API. I'm using data from EPA (行政院環境保護署環境資源資料) near my home.

## Usage

1. Setup build environment for [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos)
2. Run `git submodule update --init --recursive` for submodules this project use
3. Copy `config-sample.h` to `config.h` and edit the settings.
