#ifndef CONFIG__H
#define CONFIG__H


#define SPI_BMP_CS 15

#define I2C_DISPLAY_SDA 5
#define I2C_DISPLAY_SCL 4

#define I2C_BME1_SDA 13
#define I2C_BME1_SCL 16
// #define I2C_BMP2_SDA 5
// #define I2C_BMP2_SCL 4

#define MAX_MANUAL_VENT_TIME_S 6*3600

//Relay control pins
#define RELAY1 25
#define RELAY2 26
#define RELAY_OFF LOW
#define RELAY_ON HIGH

#define BUTTON_PIN  12
#define BUTTON_PRESSED LOW
#define BUTTON_DEBOUNCE 100

//Set the frequency of updating the relay here, in minutes. The larger the value, the less likely you will have rapid on/off cycling
#define RELAYINTERVAL 1

//Set the ventilation threshold of the DEWPOINT in degrees celcius. I recccomend it being greater than 1, as that seems to be the margin of error for these sensors
#define VENTTHRESHOLD 1.5

//Set cutoff temp for ventilation, celcius
#define TEMPCUTOFF 15


#endif