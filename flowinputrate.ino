/**************************************************************************
Flow Input Rate Device - June 2020 (updated November 2020)
 **************************************************************************/
#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int minutes = 3;

String timeMin;
String result;


long lastUpdateMillis = 0;
volatile int flow_frequency; // Measures flow sensor pulses
float l_min; // Calculated litres/min
long val;
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
void flow () // Interrupt function
{
   flow_frequency++;
}

void setup() {
  
  Serial.begin(9600);

   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   Serial.begin(9600);
   attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;
   
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}
 

int counter = 0;

void loop() {
     currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_min = (flow_frequency / 7.5); // (Pulse frequency x 1 min) / 7.5Q = flowrate in L/min
      val = (long) (l_min * 10L);      // val = 1234
      l_min = (float) val / 10.0;       // x = 1234 / 10.0 = 123.4

      flow_frequency = 0; // Reset Counter
      Serial.print(l_min, DEC); // Print litres/min
      Serial.println(" L/min");
      Serial.println(millis());
       maintainDisplay();
      result = String(l_min); 
      counter+=1;

      if (counter % 2 == 0){
        digitalWrite(LED_BUILTIN, HIGH); 
      }
      else {
        digitalWrite(LED_BUILTIN, LOW); 
      }
   }

  pinMode(LED_BUILTIN, OUTPUT);
  
   
}

//Maintains the display - by not calling delay() allows multiple tasks to take place at once
void maintainDisplay()
{
  static const unsigned long REFRESH_INTERVAL = 500; // ms
  static unsigned long lastRefreshTime = 0;
  
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
   // if (STATE != 0 || initialRefresh == false) {
                refreshDisplay();
              // initialRefresh = true;
  //  }
  }
}

// Refreshes the display with the current rate.
void refreshDisplay(void) 
{
    
  display.clearDisplay();
  display.setCursor(0,0);  
  display.setTextSize(4);             
  display.setTextColor(SSD1306_WHITE);
  display.println(result);

  
  display.setTextSize(2);           
  display.setTextColor(SSD1306_WHITE);
  //display.println(" L/min");
  display.println(counter); 
  
  display.display();
}
