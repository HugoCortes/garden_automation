/*
 * moisture_sensor.ino
 *
 * Author: Hugo Cortes
 * Description: Verify the functionality of the moisture sensor. 
 */ 


#include <Arduino.h>
#include "moistureSensor.h"

// Pin definitions
#define moist_sens_0 0




// 650 fully immersed in water.

void setup()
{
    Serial.begin(9600);   
}

void loop()
{
    int moistLevel0 = analogRead(moist_sens_0);
    Serial.println(moistLevel0);
    
    
    // moistureSensor huh;
    // int blah = huh.read(moist_sens_0);
    // Serial.println(blah);
    delay(1000);
}
