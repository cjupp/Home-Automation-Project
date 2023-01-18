

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <serial_tools.h>


#define MAX_INPUT_CHARS   10
//#define OLED_CHAR_WIDTH 
//#define OLED_CHAR_HEIGHT 


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);






void setup() 
{
  Serial.begin(115200);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.clearDisplay();

  display.display();



}

void loop() 
{
  serial_gets();


  display.clearDisplay();
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corn

  display.write(serial_buffer);
  display.display();

}
