#define SIGNAL_PIN 2
#define READ_SIGNAL 114
#define DATA_LENGTH 41

volatile int bits = 0;
volatile long lengths[DATA_LENGTH];
byte data[] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, HIGH);
  delay(1000);
  measureTemperature(SIGNAL_PIN, lengths);
  extractDataBytesFromResponse(lengths, data);
}

void loop() {
  // if (Serial.available() > 0) {
  //   byte incomingByte = Serial.read();
  //   if (incomingByte == READ_SIGNAL) {
  //     measureTemperature(SIGNAL_PIN, lengths);
  //     extractDataBytesFromResponse(lengths, data);
  //     Serial.println("_____________");
  //     for (int i = 0; i < 4; i ++) {
  //       Serial.println(data[i], BIN);
  //     }
  //   }
  // }
  // flushSerialBuffer();
  // delay(500);
}

void flushSerialBuffer() {
  if (Serial.available() == 2) {
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}

void extractDataBytesFromResponse(long response[], byte result[]) {

      for (int i = 0; i < 8; i ++) {
        int bytePosition = 0;
        byte byteInQuestion = result[bytePosition];
        byteInQuestion = byteInQuestion << 1;
        byte one = response[i] > 50 ? 1 : 0;
        byteInQuestion = byteInQuestion | one;
        Serial.println(byteInQuestion, BIN);
        result[bytePosition] = byteInQuestion;
      }

}

void measureTemperature(int interfacingPin, long result[]) {
  // signal the DHT 11 sensor to produce data
  digitalWrite(interfacingPin, LOW);
  delayMicroseconds(18000);
  digitalWrite(interfacingPin, HIGH);

  pinMode(interfacingPin, INPUT);
  int emptyResponses = 0;

  while (bits < DATA_LENGTH && emptyResponses < 5) {
    long length = pulseIn(interfacingPin, HIGH, 5000);
    if (length == 0) {
      emptyResponses ++;
      continue;
    }
    result[bits ++] = length;
  }
}
