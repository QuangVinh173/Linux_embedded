/*
 * Arduino Echo Responder
 * Reads input from host (e.g., PC/BBB via USB serial) and sends back an echo response.
 * Example: Host sends "Hello", Arduino replies "Received: Hello".
 * Works with CDC ACM (USB serial) on boards like Uno, Nano.
 */

int led = 1;
int isChanged = 0;

String inputString = "";         // Buffer for incoming data
boolean stringComplete = false;  // Flag for full line received

void setup() {
  // Initialize USB serial at 115200 baud (host can override via SET_LINE_CODING).
  Serial.begin(115200);
  pinMode(7, OUTPUT);
  digitalWrite(7, led);
  // Wait for serial connection (optional, for boards like Leonardo).
  while (!Serial) {
    ; // Wait
  }
  //Serial.println("Arduino Echo Ready! Send a message:");
  
  inputString.reserve(200);  // Pre-allocate buffer for efficiency
}

void led_changed(){
  led = 1 - led;
  digitalWrite(7, led);
  isChanged = 1;
}



void loop() {
  // Check for incoming serial data.
  while (Serial.available()) {
    //if (!isChanged) led_changed();
    // Read byte-by-byte until newline.
    char inChar = (char)Serial.read();
    inputString += inChar;
    
    // If newline (\n), mark as complete.
    if (inChar == 0x0A) {
      stringComplete = true;
      led_changed();
    //delay(1000);
    }
  }
  
  // Process if full line received.
  if (stringComplete) {
    // // Trim any trailing \r if present (Windows CRLF).
    // if (inputString.endsWith("\r")) {
    //   inputString = inputString.substring(0, inputString.length() - 1);
    // }
    
    // Echo response: Send back the input with prefix.
    //Serial.print("Received: ");

    //delay(50);
    Serial.print(inputString);
    //Serial.println();  // Add newline for clean output

    
    // Clear for next input.
    inputString = "";
    stringComplete = false;
  }
}
