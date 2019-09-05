/* 
 *  This sketch run on ESP32 with Arduino core for ESP32,
 *  demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pins depending on the request,
 *  to control the brightness of RGB LED connected to:
 *    21 : BLUE
 *    22 : GREEN
 *    23 : RED
 *    
 *    http://server_ip/rgb/rrggbb/
 *    where rr is the value set RED
 *    where gg is the value set GREEN
 *    where bb is the value set BLUE
 *    then terminate with '/'
 *  server_ip is the IP address of the ESP32, will be 
 *  printed to Serial when the module is connected.
*/
#include <WiFi.h>

const char* ssid = "";
const char* password = "";
WiFiServer server(80);

 #define MAX_LED_VALUE 255
// use first 3 channels of 16 channels (started from zero)
#define LEDC_CHANNEL_0_R  0
#define LEDC_CHANNEL_1_G  1
#define LEDC_CHANNEL_2_B  2

#define RESOLUTION 8
#define LEDC_BASE_FREQ 5000

// LED PINs
#define LED_PIN_R 23
#define LED_PIN_G 22
#define LED_PIN_B 21

/*
Because Multiple libraries were found for "WiFi.h",
Arduino IDE will: 
 Used: ...\Arduino\hardware\espressif\esp32\libraries\WiFi
 Not used: C:\Program Files (x86)\Arduino\libraries\WiFi

And exit with error:
'min' was not declared in this scope

Here's a min() to avoid that
 */
uint32_t min(uint32_t num1, uint32_t num2){
  if(num1 < num2) {
    return num1;
  } else {
    return num2;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Setup timer and attach timer to a led pins
  ledcSetup(LEDC_CHANNEL_0_R, LEDC_BASE_FREQ, RESOLUTION);
  ledcAttachPin(LED_PIN_R, LEDC_CHANNEL_0_R);
  ledcSetup(LEDC_CHANNEL_1_G, LEDC_BASE_FREQ, RESOLUTION);
  ledcAttachPin(LED_PIN_G, LEDC_CHANNEL_1_G);
  ledcSetup(LEDC_CHANNEL_2_B, LEDC_BASE_FREQ, RESOLUTION);
  ledcAttachPin(LED_PIN_B, LEDC_CHANNEL_2_B);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print(WiFi.localIP());
  Serial.println("/rgb/000000/");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(client.connected()){
    if(client.available()){
  
      // Read the first line of the request
      String req = client.readStringUntil('\r');
      Serial.println(req);
  
      // Match the request
      unsigned long valR, valG, valB;
      String subStringR, subStringG, subStringB;
      int index = req.indexOf("/rgb/");
      if(index != -1){
        if(req.charAt(index+11)=='/'){
          subStringR = req.substring(index+5, index+7);
          subStringG = req.substring(index+7, index+9);
          subStringB = req.substring(index+9, index+11);
          Serial.println("R: " + subStringR);
          Serial.println("G: " + subStringG);
          Serial.println("B: " + subStringB);
    
          char red_hex[3]; 
          strcpy(red_hex, subStringR.c_str()); 
          valR = strtoul(red_hex, NULL, 16);
          char green_hex[3]; 
          strcpy(green_hex, subStringG.c_str()); 
          valG = strtoul(green_hex, NULL, 16);
          char blue_hex[3]; 
          strcpy(blue_hex, subStringB.c_str()); 
          valB = strtoul(blue_hex, NULL, 16);
          
          Serial.println("valR: " + String(valR));
          Serial.println("valG: " + String(valG));
          Serial.println("valB: " + String(valB));
          
        } else {
          Serial.println("Not terminated with /");
          client.stop();
          return;
        }
      } else {
        Serial.println("No /rgb/ found");
        client.stop();
        return;
      }

      // Set GPIOs according to the request
      // No check valid of the requested setting
      // Probably should add a max check with the (255 - valX) inversion
      ledcWrite(LEDC_CHANNEL_0_R, min(255 - valR, MAX_LED_VALUE));
      ledcWrite(LEDC_CHANNEL_1_G, min(255 - valG, MAX_LED_VALUE));
      ledcWrite(LEDC_CHANNEL_2_B, min(255 - valB, MAX_LED_VALUE));
  
      // Prepare the response
      String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIOs of RGB is now ";
      s += String(valR) +":" + String(valG) + ":" + String(valB);
      s += "</html>\n";

      // Send the response to the client
      client.print(s);
      delay(1);
      client.stop();
      Serial.println("Client disconnected");
    }
  }
}
