#define SERIAL1_RX 19  // Serial port 1 receives pins
#define SERIAL1_TX 20  // Serial port 1 Send pins

void setup() {
  // Example Initialize serial ports 0 and 1
  Serial.begin(115200);                                       // Serial port 0 is used for debugging
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);  // Serial port 1 is initialized
  Serial.println("ESP32-S3 Serial Communication Example");
}

void loop() {
  // Check whether serial port 1 has readable data
  if (Serial1.available()) {
    String message = Serial1.readStringUntil('\n');  // Read the serial 1 message until the newline
    Serial.print("The message from serial port was received.: ");
    Serial.println(message);  // Print out to serial port 0
  }
}