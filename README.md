# r3CellarDewpointVentilation
control a ventilation fan with an ESP32 and two BME280. Display dewpoint and measurements on a SSD1306

send status over MQTT

## Hardware

- 2 Relais
- 2 BME280
- 1 ESP32 Wemos Lolin with OLED Display
- .....

### Pinout

#### Outdoor BME280
- SPI

#### Indor BME280
- I2C
- SDA: 4
- SCL: 5

#### Relais
- R1: 8
- R2: 9

#### OLED Display
- I2C
- Pins 4 and 5
- builtin

## Todo:

- Test
- improve Display
- MQTT
