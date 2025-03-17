/**
 * @title IMP project - Heart rate monitor with MAX30102 digital sensor and ESP32
 * @author Kristian Dobes - xdobes22 
 * @date 15 Dec 2023
 */

#include <Arduino.h>

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

// OLED display library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display specs
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     27
#define OLED_CS     5
#define OLED_RESET  17

// RED LED on board
#define LED_PIN 02 

// Small heart
static const unsigned char PROGMEM  small_heart_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E,  0x02, 0x10, 0x0C, 0x03, 0x10,
0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40,  0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
0x02, 0x08, 0xB8, 0x04,  0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
0x00,  0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,  };

// Big heart
static const unsigned char PROGMEM big_heart_bmp[] =
{ 0x01, 0xF0, 0x0F,  0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
0x20,  0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08,  0x03,
0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01,  0x80, 0x00, 0x14, 0x00,
0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40,  0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31,  0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
0x00, 0x60, 0xE0,  0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
0x01,  0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C,  0x00,
0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00,  0x00, 0x01, 0x80, 0x00  };

// OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Pulse sensor
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
float beatAvg;

long deltaGlob = 0; // Time between beats (global variable)

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize on board LED
  pinMode(LED_PIN, OUTPUT);

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
  delay(1000);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  long irValue = particleSensor.getIR();

  if(irValue > 7000){ // if finger is detected
    digitalWrite(LED_PIN, LOW); //Turn off Red LED

    // clear display
    display.clearDisplay();
    // draw small heart
    display.drawBitmap(5, 5, small_heart_bmp, 24, 21, WHITE);
    // display Avg BPM             
    display.setTextSize(2);
    display.setTextColor(WHITE);  
    display.setCursor(50,0);                
    display.println("BPM:");             
    display.setCursor(50,18);                
    display.println(beatAvg);
    // display HRV
    display.setTextSize(1);
    display.setCursor(0,50);                
    display.println("HRV:");             
    display.setCursor(50,50);                
    display.println(deltaGlob);
    display.setCursor(85,50);
    display.println("ms");             
    // print
    display.display();

    if (checkForBeat(irValue) == true){   // Beat found
      digitalWrite(LED_PIN, HIGH);  // Turn on Red LED on beat detection

      // clear display
      display.clearDisplay();
      // draw big heart
      display.drawBitmap(0, 0, big_heart_bmp, 32, 32, WHITE);
      // display Avg BPM             
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(50,0);                
      display.println("BPM:");             
      display.setCursor(50,18);                
      display.println(beatAvg);
      // display HRV
      display.setTextSize(1);
      display.setCursor(0,50);                
      display.println("HRV:");             
      display.setCursor(50,50);                
      display.println(deltaGlob);
      display.setCursor(85,50);                
      display.println("ms");             
      // print
      display.display();
      
      delay(100);
      
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      deltaGlob = delta;

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.print(", Delta=");
  Serial.print(deltaGlob);

  if (irValue < 50000){ //If no finger is detected
    Serial.print(" No finger?");

    // Print no finger message on display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5,10);
    display.println("Place your finger");
    display.setCursor(5,20);
    display.println("on the sensor");
    display.setCursor(10,40);
    display.setTextSize(2);
    display.println("<---");
    display.display();
  }

  Serial.println();
}