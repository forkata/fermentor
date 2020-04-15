// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

#include <sstream>
#include <cstring>

// Init Dallas on pin 6
int temperatureSensor = D6;

// The on-board LED is on pin 7
int led = D7;

DallasTemperature dallas(new OneWire(temperatureSensor));
DeviceAddress thermometerAddress;

double temperature;
int resolution;
int numberOfDevices;

void setup(){
    pinMode(led, OUTPUT);
    
    temperature = 0;
    resolution = 0;
    
    dallas.begin();
    
    dallas.setResolution(12);
    
    resolution = dallas.getResolution(thermometerAddress);
    numberOfDevices = dallas.getDeviceCount();
}

void loop(){
    // Turn LED On
    digitalWrite(led, HIGH);
    
    dallas.requestTemperatures();
    
    float temp_temperature = dallas.getTempC(thermometerAddress);
    
    if (temp_temperature != DEVICE_DISCONNECTED_C) {
        temperature = temp_temperature;
    }
    
    Particle.variable("temperature", &temperature, DOUBLE);
    Particle.variable("resolution", &resolution, INT);
    
    String data = String::format(
        "{ \"tags\" : {\"id\": \"%s\", \"location\": \"%s\"}, \"values\": {\"temperature\": %f, \"devices:\": %d } }",
        "t2",
        "fermentor 1",
        temperature,
        numberOfDevices
    );
    
    Particle.publish("temperature", data, PRIVATE);
    
    // Turn the LED Off
    digitalWrite(led, LOW);
    
    // Wait 60 sec before repeating
    delay(60000);
}