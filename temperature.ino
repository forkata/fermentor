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

int numberOfDevices;

#define DALLAS_RESOLUTION 12

void setup(){
  pinMode(led, OUTPUT);

  numberOfDevices = 0;

  dallas.begin();

  dallas.setResolution(DALLAS_RESOLUTION);

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
    DeviceAddress thermometerAddress;
    if (dallas.getAddress(thermometerAddress, i)){
      int resolution = dallas.getResolution(thermometerAddress);
      int sensorNumber = i + 1;

      String resolutionVar = "resolution" + String(sensorNumber);
      Particle.variable(resolutionVar, &resolution, INT);

      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i, DEC);

      double tempTemperature = dallas.getTempC(thermometerAddress);

      if (tempTemperature != DEVICE_DISCONNECTED_C) {
        temperatures[i]= tempTemperature;

        values.concat(
          String::format(
            " \"temperature_%d\": %f, ",
            sensorNumber,
            tempTemperature
          )
        );

        String temperatureVar = "temperature" + String(sensorNumber);
        Particle.variable(temperatureVar, &temperatures[i], DOUBLE);

        Serial.print("Temp C: ");
        Serial.print(tempTemperature);
        Serial.print(" Temp F: ");
        Serial.println(DallasTemperature::toFahrenheit(tempTemperature));
      }
    }
  }

  values.concat(
    String::format("\"devices:\": %d }", numberOfDevices)
  );

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
