#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <serial_tools.h>
#include <analog_stick.h>
#include <Simple_Menu.h>
#include <oled_hmi.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

//Wire.begin(24, 21);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define BPIN  33
#define XPIN  34
#define YPIN  35

analogStick testStick(XPIN, YPIN, BPIN, 2000);

menuFrame mainMenu;

strInput testStrMenu;
timeInput testClkMenu;
weekdayInput testWkMenu;

void getStr()
{
  char continuous = 0;
  char returnVal;
  
  do
  {
   testStrMenu.build();
   testStick.update();
   if(!testStick.held.x || continuous)
   {
     switch (testStick.status.x)
     {
      case 'l':
      testStrMenu.move('l');
      break;
  
      case 'r':
      testStrMenu.move('r');
      break;
     }
   }
   if(!testStick.held.y || continuous)
   {
     switch (testStick.status.y)
     {
      case 'u':
      testStrMenu.move('u');
      break;
  
      case 'd':
      testStrMenu.move('d');
      break;
     }
   }
   if(!testStick.held.button && testStick.status.button)
   {
      testStrMenu.sel();
   }

   delay(50);
  }while(!testStrMenu.complete);

  returnVal = testStrMenu.complete;
  Serial.print("Name: ");
  Serial.println(testStrMenu.output);

  
  testStrMenu.reset();
}


void getTime()
{
  char returnVal;
  do
  {
    testClkMenu.build();
    testStick.update();
   if(!testStick.held.x)
   {
     switch (testStick.status.x)
     {
      case 'l':
      testClkMenu.left();
      break;
  
      case 'r':
      testClkMenu.right();
      break;
     }
   }
   if(!testStick.held.y)
   {
     switch (testStick.status.y)
     {
      case 'u':
      testClkMenu.up();
      break;
  
      case 'd':
      testClkMenu.down();
      break;
     }
   }
   if(!testStick.held.button && testStick.status.button)
   {
      testClkMenu.sel();
   }
   delay(50);
    
  }while(!testClkMenu.complete);

  Serial.print("Time: ");
  Serial.print(testClkMenu.output[0], DEC);
  Serial.print(testClkMenu.output[1], DEC);
  Serial.print(testClkMenu.output[2], DEC);
  Serial.println(testClkMenu.output[3], DEC);

  returnVal = testClkMenu.complete;
  
  testClkMenu.reset();
}

void getWeek()
{
  char returnVal;
  do
  {
    testWkMenu.build();
    testStick.update();
    if(!testStick.held.x)
    {
     switch (testStick.status.x)
     {
      case 'l':
      testWkMenu.left();
      break;
    
      case 'r':
      testWkMenu.right();
      break;
     }
    }
    if(!testStick.held.button && testStick.status.button)
    {
      testWkMenu.sel();
    }
   
    delay(50);
    
  }while(!testWkMenu.complete);

  returnVal = testWkMenu.complete;
  Serial.print("Weekdays: ");
  Serial.println(testWkMenu.days, BIN);
  
  testWkMenu.reset();
  
}

void dispAnalog()
{
  Serial.print("X: ");
  Serial.println(analogRead(XPIN), DEC);

  Serial.print("Y: ");
  Serial.println(
  analogRead(YPIN), DEC);

  Serial.print("Button: ");
  Serial.println(
  testStick.status.button, DEC);

}

void dispDir()
{
  Serial.print("X direction: ");
  Serial.println(testStick.status.x);
  Serial.print("Last direction: ");
  Serial.println(testStick.lastStatus.x);
  Serial.print("Held: ");
  Serial.println(testStick.held.x, DEC);

  Serial.print("Y direction: ");
  Serial.println(testStick.status.y);
  Serial.print("Last direction: ");
  Serial.println(testStick.lastStatus.y);
  
  Serial.print("Held: ");
  Serial.println(testStick.held.y, DEC);

  Serial.print("Button: ");
  Serial.println(testStick.status.button, DEC);
  Serial.print("Last direction: ");
  Serial.println(testStick.lastStatus.button, DEC);
  Serial.print("Held: ");
  Serial.println(testStick.held.button, DEC);

  Serial.println("--------------------------------");
}




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

//-------------------------------------------------------------------------------
hmiSystemInit();
oledSystemInit();

  //Main menu
  mainMenu.addMenu("Main Menu", 0);
  mainMenu.addNode("Test Inputs", SUB_NODE, NULL);
  mainMenu.linkNode(1);

  //Submenu 1
  mainMenu.addMenu("Submenu", 1);
  mainMenu.addNode("Get String", ACT_NODE, &getStr);

  mainMenu.addNode("Get Time", ACT_NODE, &getTime);

  mainMenu.addNode("Get Weekday", ACT_NODE, &getWeek);
  




}

//

void loop() 
{
  mainMenu.build();
  testStick.update();

  
  //dispDir();
   if(!testStick.held.x)
   {
     switch (testStick.status.x)
     {
      case 'l':
      mainMenu.back();
      break;
  
      case 'r':
      mainMenu.choose();
      break;
     }
   }
   if(!testStick.held.y)
   {
     switch (testStick.status.y)
     {
      case 'u':
      mainMenu.up();
      break;
  
      case 'd':
      mainMenu.down();
      break;
     }
   }
   if(!testStick.held.button && testStick.status.button)
   {
      mainMenu.choose();
   }
   delay(50);
}


/* -------------------------------------------------------------------------------------------------------------------- |
| --------------------------------  / \  / /  / __  /  /__ __/  / ___/  / ____/ --------------------------------------- |
| -------------------------------  / /\\/ /  / /_/ /    / /    / __/    \__ \  ---------------------------------------- |
| ------------------------------  /_/  \_/  /_____/    /_/    /____/  /_____/ ----------------------------------------- |
| --------------------------------------------------------------------------------------------------------------------- |
| Last left off at:
|
| - add execute() functions for analog stick library simplify menu logic, pass in menu functions and analog stick 
|
| - make all menus use up down left right functions
|
| - add reset option to menus to allow them to be reused
|
| - use one universal display object?
|
| - save schedules to flash
|
| --------------------------------------------------------------------------------------------------------------------- */ 
