// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

#include <sstream>
#include <cstring>

// Init Dallas one-wire temperature sensors on pin 6.
const int TEMPERATURE_SENSOR_PIN = D6;

// The on-board LED is on pin 7.
const int LED_PIN = D7;

const int DALLAS_RESOLUTION = 12;

// Wait 60 sec between loop itterations.
const int LOOP_DELAY = 60000;

DallasTemperature dallas(new OneWire(TEMPERATURE_SENSOR_PIN));

// The number of temperature sensors detected on the one-wire pin.
int numberOfDevices;

void setup(){
  pinMode(LED_PIN, OUTPUT);

  numberOfDevices = 0;

  dallas.begin();

  dallas.setResolution(DALLAS_RESOLUTION);

  numberOfDevices = dallas.getDeviceCount();
}

void loop(){
  // Turn LED On
  digitalWrite(LED_PIN, HIGH);

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
  digitalWrite(LED_PIN, LOW);

  delay(LOOP_DELAY);
}
