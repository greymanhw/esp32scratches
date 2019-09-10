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
#include <string> 
#include <stdint.h>

#define MAX_LED_VALUE 255
#define RESOLUTION 8
#define LEDC_BASE_FREQ 5000

// LED PINs
const int pins[] = {23,22,21,19,18,17,25,26,27};
const int pinsLength = sizeof(pins) / sizeof(*pins);
const int ledCount = pinsLength / 3;

// [patterns defined][[led count][led channel]
unsigned long patternArray[17][ledCount][3] = {
  { 
    {0xa6,0x00,0xa6},
    {0xff,0x1c,0x00},
    {0x00,0x78,0x00},
  },
  { 
    {0x99,0x00,0x99},
    {0xdd,0x1a,0x00},
    {0x00,0x6e,0x00},
  },
  { 
    {0x8c,0x00,0x8c},
    {0xbb,0x18,0x00},
    {0x00,0x64,0x00},
  },
  { 
    {0x80,0x00,0x80},
    {0x99,0x16,0x00},
    {0x00,0x5a,0x00},
  },
  { 
    {0x73,0x00,0x73},
    {0x77,0x14,0x00},
    {0x00,0x50,0x00},
  },
  { 
    {0x66,0x00,0x66},
    {0x66,0x12,0x00},
    {0x00,0x46,0x00},
  },
  { 
    {0x59,0x00,0x59},
    {0x55,0x10,0x00},
    {0x00,0x3c,0x00},
  },
  { 
    {0x4c,0x00,0x4c},
    {0x44,0x0e,0x00},
    {0x00,0x32,0x00},
  },
  { 
    {0x40,0x00,0x40},
    {0x33,0x0c,0x00},
    {0x00,0x28,0x00},
  },
  { 
    {0x4c,0x00,0x4c},
    {0x44,0x0e,0x00},
    {0x00,0x32,0x00},
  },
  { 
    {0x59,0x00,0x59},
    {0x55,0x10,0x00},
    {0x00,0x3c,0x00},
  },
  { 
    {0x66,0x00,0x66},
    {0x66,0x12,0x00},
    {0x00,0x46,0x00},
  },
  { 
    {0x73,0x00,0x73},
    {0x77,0x14,0x00},
    {0x00,0x50,0x00},
  },
  { 
    {0x80,0x00,0x80},
    {0x99,0x16,0x00},
    {0x00,0x5a,0x00},
  },
  { 
    {0x8c,0x00,0x8c},
    {0xbb,0x18,0x00},
    {0x00,0x64,0x00},
  },
  { 
    {0x99,0x00,0x99},
    {0xdd,0x1a,0x00},
    {0x00,0x6e,0x00},
  },
  { 
    {0xa6,0x00,0xa6},
    {0xff,0x1c,0x00},
    {0x00,0x78,0x00},
  },
};

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
  Serial.println("There are " + String(ledCount) + " LEDs");

  // Setup timer and attach timer to a led pins
  int channel = 0;
  for (int i = 0; i < pinsLength; i++){
    ledcSetup(channel, LEDC_BASE_FREQ, RESOLUTION);
    ledcAttachPin(pins[channel], channel);
    channel++;
  }
}

void loop() {
      int channel;
      int patternArrayLength = sizeof(patternArray) / sizeof(*patternArray);
      for (int i = 0; i < patternArrayLength; i++){
        // i is the row of the patternArray we are working on, each row represents 3 leds
        channel = 0;
        for (int j = 0; j < ledCount; j++) {
          // j is the LED we are working on
          int patternArraySubLength = sizeof(patternArray[i]) / sizeof(*patternArray[i]);
          for (int k = 0; k < patternArraySubLength; k++) {
            ledcWrite(channel, min(255 - patternArray[i][j][k], MAX_LED_VALUE));
            channel++;
          }
        }
        delay(75);
      }
}
