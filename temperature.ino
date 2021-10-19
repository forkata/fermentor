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

  double temperatures[numberOfDevices] = { 0.0 };

  String values = String("{");

  // Loop through each device and build a payload of the data.
  for (int i = 0; i < numberOfDevices; i++) {
    // Search the wire for address
    if (dallas.getAddress(thermometerAddress, i)){
      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i, DEC);

      float tempTemperature = dallas.getTempC(thermometerAddress);
      int sensorNumber = i + 1;

      if (tempTemperature != DEVICE_DISCONNECTED_C) {
        temperatures[i]= tempTemperature;
        values = String::format(
          "%s \"temperature_%d\": %f, ",
          values.c_str(),
          sensorNumber,
          tempTemperature
        );

        String variableName = "temperature" + String(sensorNumber);
        Particle.variable(variableName, &temperatures[i], DOUBLE);

        Serial.print("Temp C: ");
        Serial.print(tempTemperature);
        Serial.print(" Temp F: ");
        Serial.println(DallasTemperature::toFahrenheit(tempTemperature));
      }
    }
  }

  values = String::format(
    "%s \"devices:\": %d }",
    values.c_str(),
    numberOfDevices
  );

  Particle.variable("resolution", &resolution, INT);
  Particle.variable("devices", &numberOfDevices, INT);

  String data = String::format(
    "{ \"tags\" : {\"id\": \"%s\", \"location\": \"%s\"}, \"values\": %s }",
    "t2",
    "fermentor 1",
    values.c_str()
  );

  Particle.publish("temperature", data, PRIVATE);

  // Turn the LED Off
  digitalWrite(led, LOW);

  // Wait 60 sec before repeating
  delay(60000);
}
