fermentor
===

Fermentation temperature monitor built using a Particle Photon connected to a
series of one-wire DS18S20 temperature probes.

The firmware supports dynamic discovery of temperature probes and reports the
data to the Particle cloud in the following format

```json
{
  "tags" : {
    "id": "t1",
    "location": "fermentor 1"
    "firmware": "0.0.1"
  },
  "values": {
    "temperature_1": 18.750000,
    "temperature_2": 18.000000,
    "devices:": 2
    }
}
```

Firmaware version 0.0.1
