// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

#include "version.h"

#include <sstream>
#include <cstring>

// Init Dallas one-wire temperature sensors on pin 6.
const int TEMPERATURE_SENSOR_PIN = D6;

// The on-board LED is on pin 7.
const int LED_PIN = D7;

const int DALLAS_RESOLUTION = 12;

// This determines how much space we allocate ahead of time
// for storing temperature readings.
const int MAX_NUMBER_OF_SENSORS = 8;

// Wait 60 sec between loop itterations.
const int LOOP_DELAY = 60000;

DallasTemperature dallas(new OneWire(TEMPERATURE_SENSOR_PIN));

// The number of temperature sensors detected on the one-wire pin.
int numberOfDevices = 0;

// Allocates the array where we store current temperature readings.
double temperatures[MAX_NUMBER_OF_SENSORS] = { 0.0 };

void setup(){
  pinMode(LED_PIN, OUTPUT);

  dallas.begin();

  dallas.setResolution(DALLAS_RESOLUTION);

  // Get the actual number of connected devices.
  numberOfDevices = dallas.getDeviceCount();

  setupParticleVariables(&numberOfDevices, temperatures);
}

void loop(){
  // Turn LED On
  digitalWrite(LED_PIN, HIGH);

  dallas.requestTemperatures();

  String values = String("{");

  // Loop through each device and build a payload of the data.
  for (int i = 0; i < numberOfDevices; i++) {
    // Search the wire for address
    DeviceAddress thermometerAddress;
    if (dallas.getAddress(thermometerAddress, i)){
      int sensorNumber = i + 1;

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

  String data = String::format(
    "{ \"tags\" : {\"id\": \"%s\", \"location\": \"%s\", \"firmware\": \"%s\"}, \"values\": %s }",
    "t2",
    "fermentor 1",
    FIRMWARE_VERSION,
    values.c_str()
  );

  Particle.publish("temperature", data, PRIVATE);

  // Turn the LED Off
  digitalWrite(LED_PIN, LOW);

  delay(LOOP_DELAY);
}

void setupParticleVariables(int *numberOfDevices, double *temperatures) {
  Particle.variable("devices", numberOfDevices, INT);

  for (int i = 0; i < *numberOfDevices; i++) {
    int sensorNumber = i + 1;

    String temperatureVar = "temperature" + String(sensorNumber);
    Particle.variable(temperatureVar, &temperatures[i], DOUBLE);
  }
}
