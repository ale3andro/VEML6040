# VEML6040
ale3andro's fork of the original Vishay VEML6040 RGBW color sensor library for Arduino [here](https://github.com/thewknd/VEML6040)

I have added two methods:

(0) String VEML6040::getColorName(void)

Reads the color from the sensor and tries to classify it in one of the following colors:

- Black 
- White 
- Grey 
- Red 
- Yellow
- Green
- Azure
- Blue 
- Magenta

(1) bool VEML6040::isColor(String color)

Given a String parameter (color), this method returns true of false if the color read from the sensor is the one matching the argument. This method makes easier the Mind+ block function.

2026.02.18

