# Fridge Controller

The aim of this project is to create a controller to handle temperature control for the beer fermentation fridge.

## Hardware Required

* Arduino UNO
* DHT11 (Temperature / Humidity sensor)
* 47K ohm resistor to connect the VIN and SIG pins of the DHT11 sensor (pull up)
* USB cable (For arduino power)
* JQC-3F-5VDC (Relay for High Voltage AC control - 5V control coil)


## Software Requirements

* Translate DHT11 signal and extract decimal value of sensor temperature

## Auxiliary Hardware

* Saleae logic analyze to help with DHT11 serial signal reading