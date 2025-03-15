#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi network name and password
const char* ssid = "elecrow888";
const char* password = "elecrow2014";

// NVIDIA API URL,If you want to use other model, need to replace the corresponding url and follow the model API agreement.
const char* invoke_url = "https://integrate.api.nvidia.com/v1/chat/completions";

// Your authorization token , the previous step in the web page to apply and save
const char* authorization_header_value = "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";

void setup() 
{
  // Initializing serial port
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Create an HTTP POST request
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;
    // Setting the target server
    http.begin(invoke_url);
    // Set HTTP header
    http.addHeader("Authorization", "Bearer " + String(authorization_header_value));
    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");

    // Create POST data，the question is embedded here
    String httpRequestData = "{\"messages\":[{\"role\":\"user\",\"content\":\"Write a limerick about the wonders of GPU computing.\"}],\"stream\":true,\"model\":\"meta/llama-3.1-405b-instruct\",\"max_tokens\":1024,\"presence_penalty\":0,\"frequency_penalty\":0,\"top_p\":0.7,\"temperature\":0.2}";

    // Send an HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // Check the HTTP response code
    if (httpResponseCode > 0) 
    {
      // Obtain the server response and print it out on the serial port
      String response = http.getString();  
      Serial.println(httpResponseCode);

      // Used to store the split string
      String splitStrings[50];  // Split up at most 50 string sand also can set the length of the text you want to output

      // Text starts and ends with indexes
      int splitIndex = 0;
      int startIndex = 0;
      int endIndex = 0;

      //Because the URL returns a useful chunk of data for transmission, the string should be cut first. If you are using a streaming url, you can omit this step and output directly
      while ((endIndex = response.indexOf("data:", startIndex)) != -1) 
      {
        // Find the location of the next "data:"
        int nextIndex = response.indexOf("data:", endIndex + 1);

        // If the next "data:" cannot be found, the remaining string is taken as the last split string
        if (nextIndex == -1) 
        {
          splitStrings[splitIndex++] = response.substring(startIndex);
        } else 
        {
          // Extracts the string between the "data:" tags
          splitStrings[splitIndex++] = response.substring(startIndex, nextIndex);
          // Update startIndex to the location of the next "data:"
          startIndex = nextIndex;
        }
      }

        Serial.println("LLAMA 3 answer:");

      // The serial port prints the text of the information we need
      for (int i = 0; i < splitIndex; i++) 
      {
        // Find the "content" field
        int contentStart = splitStrings[i].indexOf("content\":\"") + 10;
        int contentEnd = splitStrings[i].indexOf("\"}", contentStart);

        // Extract the contents of the "content" field
        String content = splitStrings[i].substring(contentStart, contentEnd);

        // Print the contents of the content field
        if (contentStart != 9)
        {
          Serial.printf("%s", content.c_str());
        }
      }

    } 
    //Access failure Information is displayed
    else     
    {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    //Close junction
    http.end();
  }
}

void loop() {
  // Nothing needs to be done here
}
