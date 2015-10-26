
/*
 * moisture_sensor.h
 *
 * Created: 10/25/2015 3:47:41 PM
 *  Author: HUGO1
 */ 

#ifndef moistureSensor_h
#define moistureSensor_h

#include <Arduino.h>

class moistureSensor
{
    public:
        moistureSensor();
        int read(int analogPin);  
};

#endif