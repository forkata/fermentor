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

int resolution;
int numberOfDevices;

#define DALLAS_RESOLUTION 12

void setup(){
    pinMode(led, OUTPUT);
    
    resolution = 0;
    numberOfDevices = 0;
    
    dallas.begin();
    
    dallas.setResolution(DALLAS_RESOLUTION);
    
    resolution = dallas.getResolution(thermometerAddress);
    numberOfDevices = dallas.getDeviceCount();
}

void loop(){
    // Turn LED On
    digitalWrite(led, HIGH);
    
    dallas.requestTemperatures();
    
    double temperatures[numberOfDevices] = { };
    
    // Loop through each device and build a payload of the data.
    for (int i = 0; i < numberOfDevices; i++) {
        // Search the wire for address
        if (dallas.getAddress(thermometerAddress, i)){
    		// Output the device ID
    		Serial.print("Temperature for device: ");
    		Serial.println(i, DEC);

            float temp_temperature = dallas.getTempC(thermometerAddress);

            if (temp_temperature != DEVICE_DISCONNECTED_C) {
                temperatures[i]= temp_temperature;
                 Serial.print("Temp C: ");
                 Serial.print(temp_temperature);
                 Serial.print(" Temp F: ");
                 Serial.println(DallasTemperature::toFahrenheit(temp_temperature));
            }
        }
    }
    
    Particle.variable("temperature1", &temperatures[0], DOUBLE);
    Particle.variable("temperature2", &temperatures[1], DOUBLE);
    Particle.variable("resolution", &resolution, INT);
    Particle.variable("devices", &numberOfDevices, INT);
    
    String data = String::format(
        "{ \"tags\" : {\"id\": \"%s\", \"location\": \"%s\"}, \"values\": {\"temperature_1\": %f, \"temperature_2\": %f, \"devices:\": %d } }",
        "t2",
        "fermentor 1",
        temperatures[0],
        temperatures[1],
        numberOfDevices
    );
    
    Particle.publish("temperature", data, PRIVATE);
    
    // Turn the LED Off
    digitalWrite(led, LOW);
    
    // Wait 60 sec before repeating
    delay(60000);
}