#define SIGNAL_PIN 2
#define READ_SIGNAL 114
#define DATA_LENGTH 40

volatile int bits = 0;
volatile long lengths[DATA_LENGTH];
byte data[] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, HIGH);
}

void loop() {
  if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    Serial.println(incomingByte);
    if (incomingByte == READ_SIGNAL) {
      measureTemperature(SIGNAL_PIN, lengths);
      Serial.println(sizeof(lengths));
      for (int i = 0; i < DATA_LENGTH; i ++) {
        Serial.println(lengths[i]);
      }
    }
  }
  flushSerialBuffer();
  delay(500);
}

void flushSerialBuffer() {
  if (Serial.available() == 2) {
    while (Serial.available() > 0) {
      Serial.read();
    }
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
