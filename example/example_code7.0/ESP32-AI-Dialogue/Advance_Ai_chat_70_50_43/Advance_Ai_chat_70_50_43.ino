#include <WiFi.h>              // WiFi library for connecting and managing WiFi networks
#include <WiFiManager.h>       // Used to simplify the WiFi configuration process
#include <WebSocketsClient.h>  // It is used to implement WebSocket client functions
#include <base64.h>            // Used for Base64 encoding and decoding
#include <ArduinoJson.h>       // Used to process JSON data

// Include the I2S driver library, which provides functions and structures
// for interacting with the Inter - IC Sound (I2S) interface on the ESP32.
// The I2S interface is commonly used for audio input and output operations,
// such as connecting microphones and speakers.
#include <driver/i2s.h>

#include <Button.h>
#include "ChatDialog.h" 
#include "Arduino.h"

ChatDialog chatDialog;
// bool isAIReplying = false;

#define BUTTON_PIN 0        // Button connected to pin 0
Button button(BUTTON_PIN);  // Create a Button object

// Custom initialization variables
const char* websocket_server = "192.168.2.157";  // Server ip address----Please change to your computer's IP address

const int websocket_port = 8765;
const char* websocket_path = "/";
String macAddress;           // Define global variables
WebSocketsClient webSocket;  // Define the WebSocket client

// I2S configuration
#define I2S_MIC_NUM I2S_NUM_0
#define I2S_SPK_NUM I2S_NUM_1
#define SAMPLE_RATE 16000  // Sampling rate
#define SAMPLE_SIZE 1024   // Number of samples per transfer

bool isOpenMic = false;                        // Initialize the definition of microphone on
bool isOpenSpk = false;                        // Initialize the definition of microphone off
unsigned long isOpenMic_true_time = millis();  // Current time when the microphone is turned on

// I2S microphone configuration structure
const i2s_config_t i2s_config_mic = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),  // Master mode and receive mode
  .sample_rate = SAMPLE_RATE,                           // Sampling rate
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // Number of bits per sample
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,          // Only use the left channel
  .communication_format = I2S_COMM_FORMAT_I2S,          // Communication format
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,             // Interrupt allocation flag
  .dma_buf_count = 8,                                   // Number of DMA buffers
  .dma_buf_len = SAMPLE_SIZE,                           // Length of DMA buffer
  .use_apll = false,                                    // Do not use APLL
  .tx_desc_auto_clear = false,                          // Do not automatically clear transmit descriptors
  .fixed_mclk = 0                                       // Fixed MCLK frequency
};

// I2S microphone pin configuration structure
const i2s_pin_config_t pin_config_mic = {
  .bck_io_num = 19,                   // Serial-Data Clock for I2S interface. For 7.0-inch, 5.0-inch, and 4.3-inch devices, it is 19 /---- For 3.5-inch, 2.8-inch, and 2.4-inch devices, it is 9
  .ws_io_num = 2,                     // Serial Data-Word Select for I2S Interface. For 7.0-inch, 5.0-inch, and 4.3-inch devices, it is 2 /---- For 3.5-inch, 2.8-inch, and 2.4-inch devices, it is 3
  .data_out_num = I2S_PIN_NO_CHANGE,  // Data output pin remains unchanged
  .data_in_num = 20                   // Serial-Data Output for I2S interface. For 7.0-inch, 5.0-inch, and 4.3-inch devices, it is 20 /---- For 3.5-inch, 2.8-inch, and 2.4-inch devices, it is 10
};

// I2S speaker configuration
const i2s_config_t i2s_config_spk = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),  // Set to I2S master mode and transmit mode (for speaker output)
  .sample_rate = 12000,                                 // Set sample rate to 12000Hz (adjust according to audio data format)
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // 16 bits per sample
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,         // Use dual channels (left and right)
  .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),  // Set communication format to I2S, MSB first
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,             // Set interrupt priority to 1
  .dma_buf_count = 8,                                   // Set number of DMA buffers to 8
  .dma_buf_len = 1024,                                  // Each DMA buffer length is 1024
  .use_apll = false,                                    // Do not use APLL (audio PLL)
  .tx_desc_auto_clear = true,                           // Automatically clear transmit descriptors
  .fixed_mclk = 0                                       // Do not set fixed master clock frequency
};

// I2S speaker pin configuration
const i2s_pin_config_t pin_config_spk = {
  .bck_io_num = 5,    // I2S bit clock pin, BCLK. For 7.0-inch, 5.0-inch, and 4.3-inch devices, it is 5 /---- For 3.5-inch, 2.8-inch, and 2.4-inch devices, it is 13
  .ws_io_num = 6,     // I2S left/right channel frame clock pin, LRCLK. For 7.0-inch, 5.0-inch, and 4.3-inch devices, it is 6 /---- For 3.5-inch, 2.8-inch, and 2.4-inch devices, it is 11
  .data_out_num = 4,  // I2S serial data input pin. DATA. For 7.0-inch, 5.0-inch, and 4.3-inch devices, it is 4 /---- For 3.5-inch, 2.8-inch, and 2.4-inch devices, it is 12
  .data_in_num = I2S_PIN_NO_CHANGE
};

QueueHandle_t queue;  // Create queue handle

void Task1(void *pvParameters) {
    String *receivedDataPtr;  // Used to store String from queue
    while (1) {
        // Try to receive data from queue (non-blocking)
         if (xQueueReceive(queue, &receivedDataPtr, 0) == pdPASS) {
            chatDialog.addMessage(*receivedDataPtr, true);
            delete receivedDataPtr; // Free memory to prevent leaks
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);  // Avoid task hogging CPU
    }
}

void setup() {
  Serial.begin(115200);  // Initialize the serial port
  init_screen();         // Initialize the screen

  // Create a queue with capacity of 1000
  queue = xQueueCreate(1000, sizeof(String));
    
  if (queue == NULL) {
      Serial.println("Queue creation failed");
      while (1); // Prevent further execution
  }

  // Create Task1 to monitor queue
  xTaskCreatePinnedToCore(
      Task1,    // Task function
      "Task1",  // Task name
      10000,    // Task stack size
      NULL,     // Task parameters
      1,        // Task priority
      NULL,     // Task handle
      0         // Run on Core 0
  );

  wifiServer();          // Connect to the Wi-Fi network
  connect_ws();          // Connect to the WebSocket server
  init_MIC();            // Initialize the microphone
  init_SPK();            // Initialize the speaker
  button.begin();        // Initialize button
}


void loop() {

  chatDialog.update();
  buttonWakeUpTask();
  change_status();
  webSocket.loop();     // Start the WebSocket event loop
  record_send_audio();  // Record and send audio

}

//----------------------------------------------------------------------------- Screen display ----------------------------------------------------------
// Function to initialize the screen and related components
void init_screen() {
  pinMode(19, OUTPUT);
  chatDialog.begin();
}
//----------------------------------------------------------------------------- Screen display ----------------------------------------------------------

// Button wake-up event
void buttonWakeUpTask() {
  static unsigned long interruptStartTime = 0;  // Record interrupt start time
  static bool waitingToOpenMic = false;         // Mark whether you are waiting to turn on the microphone

  button.read();  // Read button status

  if (button.pressed()) {
    if (isOpenSpk && !isOpenMic) {
      Serial.print("Interrupt!");
      isOpenSpk = false;
      interruptStartTime = millis();  // Record current time
      waitingToOpenMic = true;        // Flag start wait
      String jsonString_i = createJsonString("interrupt_audio", "");
      webSocket.sendTXT(jsonString_i);

    } else if (!isOpenSpk && !isOpenMic) {
      Serial.print("Wake up!");
      // Send wake up signal
      String jsonString_i = createJsonString("wake_up", "");
      webSocket.sendTXT(jsonString_i);
      isOpenSpk = true;
    }
  }

  // Check if you have waited 1 second
  if (waitingToOpenMic && (millis() - interruptStartTime >= 1000)) {
    isOpenMic = true;
    isOpenMic_true_time = millis();
    waitingToOpenMic = false;  // Cancel wait
  }
}

// * Connect to the Wi-Fi network
void wifiServer() {
  WiFiManager manager;
  // Get the MAC address of the ESP and assign it to the global variable
  macAddress = WiFi.macAddress();
  // Remove the colons
  macAddress.replace(":", "");
  Serial.print("The address is:");
  Serial.println(macAddress);
  // manager.resetSettings();  // Reset the WiFi settings
  String APName = "AI-" + macAddress;
  manager.autoConnect(APName.c_str());
  // Connect to the Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi is connected");
}

// * Connect to the WebSocket server
void connect_ws() {
  webSocket.begin(websocket_server, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
}

// * WebSocket event handling function
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;

    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      // Play opening remarks
      {
        String jsonString = createJsonString("open_word", "");
        webSocket.sendTXT(jsonString);
        Serial.println("WebSocket connected");
        Serial.println("Starting, ready to record");
        isOpenMic = true;
        isOpenMic_true_time = millis();
      }
      break;

    case WStype_TEXT: {
        Serial.printf("Received text data: %s\n", payload);
        String rcv_word = (char*)payload;  // Variable in case block

        if (rcv_word.startsWith("USER:")) {
            chatDialog.addMessage(rcv_word.substring(5), false);
        } else if (rcv_word.startsWith("AI:")) {
            String *message = new String(rcv_word.substring(3));
                // Send data to queue
              if (xQueueSend(queue, &message, portMAX_DELAY) != pdPASS) {
                  Serial.println("Queue full, cannot send");
              }
        } else if (rcv_word == "close_mic") {
            isOpenMic = false;
            Serial.println("Microphone turned off");
            isOpenSpk = true;
            Serial.println("Speaker turned on");
        } else if (rcv_word == "finish_tts") {
            delay(1000);
            String jsonString = createJsonString("re_process_audio", "");
            webSocket.sendTXT(jsonString);
            Serial.println("Starting ASR again, ready to record");
            isOpenSpk = false;
            isOpenMic = true;
            isOpenMic_true_time = millis();
        }
        break;
    }

    case WStype_BIN: {
        size_t bytes_written;
        Serial.printf("Received binary data, length: %d\n", length);
        if (isOpenSpk) {
            i2s_write(I2S_SPK_NUM, payload, length, &bytes_written, portMAX_DELAY);
        }
        break;
    }
  }
}

// * Initialize the I2S microphone
void init_MIC() {
  i2s_driver_install(I2S_MIC_NUM, &i2s_config_mic, 0, NULL);
  i2s_set_pin(I2S_MIC_NUM, &pin_config_mic);
  i2s_zero_dma_buffer(I2S_MIC_NUM);
}

// * Initialize the I2S speaker
void init_SPK() {
  i2s_driver_install(I2S_SPK_NUM, &i2s_config_spk, 0, NULL);
  i2s_set_pin(I2S_SPK_NUM, &pin_config_spk);
  i2s_zero_dma_buffer(I2S_SPK_NUM);
}

// Record and send audio
void record_send_audio() {
  static unsigned long lastTime = 0;
  static unsigned long lastVoiceTime = 0;
  static bool isVoiceActive = false;  // Flag: whether there is voice
  unsigned long currentTime = millis();

  if (isOpenMic && millis() - isOpenMic_true_time > 10000 && !isVoiceActive) {
    Serial.print("No voice for 10 seconds, enter sleep mode. Press the boot button to wake up.");
    isOpenMic = false;
    String jsonString = createJsonString("timeout_no_stream", "");
    webSocket.sendTXT(jsonString);
  }

  // Check if it's time for the next sampling interval
  if (currentTime - lastTime >= (1000 / SAMPLE_RATE * SAMPLE_SIZE)) {
    lastTime = currentTime;

    size_t bytesRead;
    uint8_t i2sData[SAMPLE_SIZE * 2] = { 0 };  // 16-bit samples, 2 bytes per sample
    i2s_read(I2S_MIC_NUM, i2sData, SAMPLE_SIZE * 2, &bytesRead, portMAX_DELAY);

    if (bytesRead > 0 && isOpenMic) {
      // Calculate the average absolute value of the audio signal to detect voice
      int16_t* samples = (int16_t*)i2sData;
      long sum = 0;
      for (int i = 0; i < SAMPLE_SIZE; i++) {
        sum += abs(samples[i]);
      }
      float average = sum / (float)SAMPLE_SIZE;

      if (average > 200) {  // Set a threshold to determine if there is human voice
        Serial.println("Voice detected");
        lastVoiceTime = currentTime;  // Update the time of the last voice
        isVoiceActive = true;         // Set the flag
      } else {
        if (isVoiceActive && currentTime - lastVoiceTime > 2000) {
          Serial.println("No voice for 2 seconds");
          String jsonString = createJsonString("record_stream", "0x04");
          webSocket.sendTXT(jsonString);
          isVoiceActive = false;  // Reset the flag
          isOpenMic = false;      // Set isOpenMic to false
          isOpenSpk = true;
          Serial.println("Turn on the speaker");
        }
      }
      // Convert the binary audio data to Base64 encoding
      String base64Data = base64::encode(i2sData, bytesRead);
      Serial.println("Sending audio stream");
      String jsonString = createJsonString("record_stream", base64Data);
      webSocket.sendTXT(jsonString);
    }
  }
}

// Create a JSON data body
String createJsonString(const String& type, const String& data) {
  // Build a JSON object
  StaticJsonDocument<300> jsonDoc;
  // Create a nested JSON structure
  jsonDoc["event"] = type;
  jsonDoc["mac_address"] = macAddress;
  jsonDoc["data"] = data;  // Use data as a sub-item under macAddress
  // Serialize the JSON object into a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return jsonString;  // Return the serialized JSON string
}

// Change status logic
void change_status() {
  if (!isOpenMic && !isOpenSpk) {
      chatDialog.showMicOrSpk(3);
//(starting x-coordinate, starting y-coordinate, image length, image width). The image length and image width can be viewed by right-clicking on the image and selecting "Properties"
  } else if (isOpenMic && !isOpenSpk) {
      chatDialog.showMicOrSpk(1);
  } else if (!isOpenMic && isOpenSpk) {
      chatDialog.showMicOrSpk(2);
  }
}