# ESP8266 Heating Valve

An Homie (https://github.com/marvinroger/homie-esp8266) based program to control homemade heating valves using a Wemos D1 mini, an ULN2003 chip, and a 28BYJ-48 stepper motor.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

This project is genrated with PlatformIO, so please install it before.

### Installing

```
git clone https://github.com/Neonox31/esp8266-floor-heating-valve.git
cd esp8266-floor-heating-valve
pio init
```

### Configure

```
set -x DEVICE_NAME "My Device Name"
set -x DEVICE_ID my-device-id
set -x WIFI_SSID my-ssid
set -x WIFI_PASSWORD my-password
envsubst < homie-config.json > private-config.json
curl -X PUT http://192.168.123.1/config --header "Content-Type: application/json" -d @private-config.json
```
