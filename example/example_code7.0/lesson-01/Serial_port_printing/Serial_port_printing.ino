int counter = 0; // Define a counter variable

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  // Print "Hello, World!" and the value of the counter
  Serial.print("Hello, World! "); // Print "Hello, World!"
  Serial.print("--- "); // Add space between "Hello, World!" and the number
  Serial.println(counter); // Print the value of the counter

  counter++; // Increment the counter
  delay(1000); // Output every 1 second
}
