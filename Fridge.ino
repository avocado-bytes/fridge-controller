/**
 * 
 * NOTE 47K resistor is used to connect the VIN and SIG terminals of the DHT11 sensor to ensure HIGH state of the SIG line by default.
 * 
 **/

#define SIGNAL_PIN 2
#define READ_SIGNAL 114
#define DATA_LENGTH 40

volatile int bits = 0;
volatile long lengths[DATA_LENGTH];
byte data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/**
 * This function sets up the environment. The pin connected to the signal pin of the DHT 11 has to be setup as OUTPUT and be set to HIGH.
 * Serial communication is setup for debugging purposes
 **/
void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, HIGH);
}

/**
 * 
 * The loop function is just waiting for the trigger word - in this case the 'r' letter that will trigger the measurements.
 * This will be changed in the future.
 * 
 **/
void loop() {
  if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    if (incomingByte == READ_SIGNAL) {
      startMeasurements();
    }
  }
  flushSerialBuffer();
  delay(500);
}

/**
 * 
 * The entry point of the measurements.
 * This function will reset the values in the <b>data</b> array.
 * Then it will trigger the pulse width measurements.
 * Subsequently it will call the translation method and print the results.
 * 
 **/
void startMeasurements() {
  for (int i = 0; i < 5; i ++) {
    data[i] = 0xFF;
  }
  measureTemperature(SIGNAL_PIN, lengths);
  extractDataBytesFromResponse(lengths, data);
  printResults();
}

/**
 * This utility function is used to get rid of the remaining two bytes always present in the serial buffer after input from computer keyboard.
 * I suspect these are whitespace characters added by hitting enter at the end of the command but I have not investigated this further.
 **/
void flushSerialBuffer() {
  if (Serial.available() == 2) {
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}

/**
 * Simple formatting function to beautify the data gathered for output.
 **/
void printResults() {
  bool parityOk = (data[0] + data[2]) == data[4];
  String parityInformation = parityOk ? "Parity OK" : "Data corrupted!";
  String relativeHumidity = "Humidity: " + String(data[0]) + " %";
  String temperature = "Temperature: " + String(data[2]) + " °C";
  Serial.println(parityInformation);
  Serial.println(relativeHumidity);
  Serial.println(temperature);
}

/**
 * 
 * Main aim of this function is to populate the <b>data</b> array with the translated byte values of the serial
 * data emitted by the DHT11 sensor. 
 * The values in the <b>data</b> array represent the following
 * <ol>
 *  <li>Humidity - integral value</li>
 *  <li>Humidity - decimal value</li>
 *  <li>Temperature - integral value</li>
 *  <li>Temperature - decimal value</li>
 *  <li>Parity check</li>
 * </ol>
 * 
 * This utility function will make use of the data gathered in the <b>lengths</b> array.
 * The <b>lengths</b> array contains the lengths of every HIGH pulse emitted by the DHT11 sensor.
 * This function will translate the lengths into their respective bit values. 0 bits are usually 25us long
 * while 1 bits are usually 72us long. 50us has been chosen as threshold when translating the pulse length values.
 * Therefore any pulse shorter than or equal to 50us willl be interpreted as 0 while any pulse longer then 50us
 * will be interpreted as 1. 
 *  
 **/
void extractDataBytesFromResponse(volatile long response[], byte result[]) {
  int lastBytePosition = 0;
  for (int i = 0; i < DATA_LENGTH; i ++) {
    int bytePosition = i / 8;
    if(lastBytePosition != bytePosition) {
      lastBytePosition = bytePosition;
    }
    byte byteInQuestion = result[bytePosition];
    byte bitValue = response[i] > 50 ? 1 : 0;
    byteInQuestion = byteInQuestion << 1;
    byteInQuestion = byteInQuestion | bitValue;
    result[bytePosition] = byteInQuestion;
  }
}

/**
 * 
 * This is the entry function that will launch the data read from the DHT11 sensor.
 * It will send the initiation signal to the sensor.
 * Next it will skip the ACK bit of the DHT11 response.
 * Next it will store the lengths of all of the pulses emitted by the DHT11 sensor into 
 * the <b>lengths</b> array.
 * 
 **/
void measureTemperature(int interfacingPin, volatile long result[]) {
  digitalWrite(interfacingPin, LOW);
  delayMicroseconds(18000);
  digitalWrite(interfacingPin, HIGH);

  pinMode(interfacingPin, INPUT);
  int emptyResponses = 0;

  bool ackBit = true;

  // continue the loop until the data length is reached or 5 consecutive empty responses have been received indicating an error in communication
  while (bits < DATA_LENGTH && emptyResponses < 5) {
    long length = pulseIn(interfacingPin, HIGH, 5000);
    if (length == 0) {
      emptyResponses ++;
      continue;
    }
    // skip the ack bit - in case of the nack the 5 empty responses will handle the loop termination
    if (ackBit) {
      ackBit = false;
      continue;
    }
    result[bits ++] = length;
  }
}
