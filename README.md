# r3CellarDewpointVentilation
control a ventilation fan with an ESP32 and two BME280. Display dewpoint and measurements on a SSD1306

send status over MQTT

## Hardware

- 2 Relais
- 2 BME280
- 1 ESP32 Wemos Lolin with OLED Display
- 1 Button

### Pinout

#### ESP32
- 25  --> Relay IN1
- 26  --> Relay IN2
- 12  --> Button (pulls to GND)
- GND --> Button
- 13  --> I2C SDA Bus 2
- 16  --> I2C SCL Bus 2
- 4   --> (I2C Display)
- 5   --> (I2C Display)
- GND --> Relay GND
- 5V  --> Relay VCC
- GND --> BME280 #1 GND
- GND --> BME280 #2 GND
- 3V3 --> BME280 #1 3V3
- 3V3 --> BME280 #2 3V3

- Power -> USB

#### Outdoor BME280
- I2C Bus 2

#### Indor BME280
- I2C Bus 2

#### Relais

#### OLED Display
- I2C Bus 1
- Pins 4 and 5
- builtin
