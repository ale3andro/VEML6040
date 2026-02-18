/*

The MIT License (MIT)

Copyright (c) 2015 thewknd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Wire.h"
#ifndef __MATH_H
#include <math.h>
#endif
#include "veml6040.h"
#include <Arduino.h>


VEML6040::VEML6040(void) {
  
}

bool VEML6040::begin(void) {
  bool sensorExists = false;
  Wire.begin();
  Wire.beginTransmission(VEML6040_I2C_ADDRESS);
  if (Wire.endTransmission() == 0) {
    sensorExists = true;
  }
  return sensorExists;
}

void VEML6040::setConfiguration(uint8_t configuration) {
  Wire.beginTransmission(VEML6040_I2C_ADDRESS);  
  Wire.write(COMMAND_CODE_CONF); 
  Wire.write(configuration); 
  Wire.write(0);
  Wire.endTransmission(); 
  lastConfiguration = configuration;
}

uint16_t VEML6040::read(uint8_t commandCode) {
  uint16_t data = 0; 
  
  Wire.beginTransmission(VEML6040_I2C_ADDRESS);
  Wire.write(commandCode);
  Wire.endTransmission(false);
  Wire.requestFrom(VEML6040_I2C_ADDRESS,2);
  while(Wire.available()) 
  {
    data = Wire.read(); 
    data |= Wire.read() << 8;
  }
  
  return data; 
}

uint16_t VEML6040::getRed(void) {
  return(read(COMMAND_CODE_RED));
}

uint16_t VEML6040::getGreen(void) {
  return(read(COMMAND_CODE_GREEN));
}

uint16_t VEML6040::getBlue(void) {
  return(read(COMMAND_CODE_BLUE));
}

uint16_t VEML6040::getWhite(void) {
  return(read(COMMAND_CODE_WHITE));
}

float VEML6040::getAmbientLight(void) {
  uint16_t sensorValue; 
  float ambientLightInLux;
  
  sensorValue = read(COMMAND_CODE_GREEN);
  
  switch(lastConfiguration & 0x70) {
  
    case VEML6040_IT_40MS:    ambientLightInLux = sensorValue * VEML6040_GSENS_40MS;
                              break;
    case VEML6040_IT_80MS:    ambientLightInLux = sensorValue * VEML6040_GSENS_80MS;
                              break;
    case VEML6040_IT_160MS:   ambientLightInLux = sensorValue * VEML6040_GSENS_160MS;
                              break;
    case VEML6040_IT_320MS:   ambientLightInLux = sensorValue * VEML6040_GSENS_320MS;
                              break;
    case VEML6040_IT_640MS:   ambientLightInLux = sensorValue * VEML6040_GSENS_640MS;
                              break; 
    case VEML6040_IT_1280MS:  ambientLightInLux = sensorValue * VEML6040_GSENS_1280MS; 
                              break;   
    default:                  ambientLightInLux = -1;
                              break;                             
  } 
  return ambientLightInLux;
}

uint16_t VEML6040::getCCT(float offset) {
  uint16_t red,blue,green;
  float cct,ccti;
  
  red = read(COMMAND_CODE_RED);
  green = read(COMMAND_CODE_GREEN);
  blue = read(COMMAND_CODE_BLUE);
  
  ccti = ((float)red-(float)blue) / (float)green;
  ccti = ccti + offset; 
  cct = 4278.6 * pow(ccti,-1.2455);
  
  return((uint16_t)cct);
}

String VEML6040::getColorName(void) {
    
  uint16_t r, b, g, w;
  
  r = read(COMMAND_CODE_RED);
  g = read(COMMAND_CODE_GREEN);
  b = read(COMMAND_CODE_BLUE);
  w = read(COMMAND_CODE_WHITE);

  float rd = (float)r;
  float gd = (float)g;
  float bd = (float)b;

  float maxVal = rd;
  if (gd > maxVal) maxVal = gd;
  if (bd > maxVal) maxVal = bd;

  float minVal = rd;
  if (gd < minVal) minVal = gd;
  if (bd < minVal) minVal = bd;

  float delta = maxVal - minVal;

  // 1. Adjusted Black Threshold (approx 0.5% of max)
  if (w < 300) return "Black";

  // 2. Check for White/Grey
  float saturation = (maxVal == 0) ? 0 : (delta / maxVal);
  
  if (saturation < 0.15) { 
      // Adjusted White Threshold (High intensity)
      return (w > 10000) ? "White" : "Grey";
  }

  // 3. Hue calculation remains identical (it's ratio-based!)
  float h = 0;
  if (delta != 0) {
      if (maxVal == rd) h = 60 * fmod(((gd - bd) / delta), 6);
      else if (maxVal == gd) h = 60 * (((bd - rd) / delta) + 2);
      else if (maxVal == bd) h = 60 * (((rd - gd) / delta) + 4);
  }
  if (h < 0) h += 360;

  // 4. Return name based on Hue
  if (h <= 45 || h >= 335) return "Red";
  if (h > 45 && h <= 65)  return "Yellow";
  if (h > 65 && h <= 150)  return "Green";
  if (h > 150 && h <= 195)  return "Azure";
  if (h > 195 && h <= 260)  return "Blue";
  if (h > 260 && h < 330)  return "Magenta";
}

bool VEML6040::isColor(String color) {
  String sensorColor = getColorName();
  return (sensorColor==color) ? true : false;
}