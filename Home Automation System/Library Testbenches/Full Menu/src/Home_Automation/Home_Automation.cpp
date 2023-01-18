/*--------------------------------------------------------------------------------------------------------------------
| Home_Automation.cpp: Organizes and houses all global objects and libraries needed for home automation and heating 
|
| Created by:     Cameron Jupp
| Date Started:   January 7, 2023
--------------------------------------------------------------------------------------------------------------------*/ 
#include "Home_Automation.h"

#include "Arduino.h"

#include "Active_WiFi.h"
#include "Analog_Stick.h"
#include "EEPROM_Manager.h"
#include "Events_Scheduling.h"
#include "HTTP_Server.h"
#include "OLED_Tools.h"
#include "OLED_HMI.h"
#include "Serial_Tools.h"
#include "Simple_Menu.h"
#include "Real_Timekeeping.h"

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------- / ___/  / /     / __  /  / _ /    / _\      / /     / ____/ ----------------------------- //
// ------------------------- / /_//  / /__   / /_/ /  / _  \   / /_\\    / /__    \__ \  ------------------------------ //
// ------------------------ /____/  /____/  /_____/  /_____/  /_/   \\  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// Declare the analog stick used for the project using the defined pins and threshold
analogStick joystick(XPIN, YPIN, BPIN, 2000);

// Declare the menu frame object for the main menu
menuFrame mainMenu;

// Declare the event handler for the heating
eventHandler heatEvents;

// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------- / ___/  / /     / _\      / ____/  / ____/  / ___/  / ____/ -------------------------- //
// ---------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  --------------------------- //
// --------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ---------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //



// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

char HA_SystemInit();
{
  Serial.begin(115200);

  //wifiSystemInit();

  // Initialize and clear the OLED display
  oledSystemInit();

  // Initialize and clear the LCD display
  lcdSystemInit();


    // Call initializations for all libraries here

    // return -1 if something does not initialize properly
}


void checkSched(void * parameters)
{
  for(;;)
  {
    Serial.println("Checking Events...");
    Serial.println(timeinfo.tm_hour, DEC);
    Serial.println(timeinfo.tm_min, DEC);
    testEvents.check(timeinfo.tm_hour, timeinfo.tm_min, 0b00010000);
    vTaskDelay(30000 / portTICK_PERIOD_MS);
  }

}

void menuThread()
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
   delay(100);
}

char * getStr()
{
  testStrMenu.reset();
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

   delay(100);
  }while(!testStrMenu.complete);

  returnVal = testStrMenu.complete;
  Serial.print("Name: ");
  Serial.println(testStrMenu.output);

  
  return testStrMenu.output;
}

clkTime * getTime()
{
  testClkMenu.reset();
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
   delay(100);
    
  }while(!testClkMenu.complete);

  Serial.print("Time: ");
  Serial.print(testClkMenu.output[0], DEC);
  Serial.print(testClkMenu.output[1], DEC);
  Serial.print(testClkMenu.output[2], DEC);
  Serial.println(testClkMenu.output[3], DEC);

  return testClkMenu.accept();
}

char getWeek()
{
  testWkMenu.reset();
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
   
    delay(100);
    
  }while(!testWkMenu.complete);

  returnVal = testWkMenu.complete;
  Serial.print("Weekdays: ");
  Serial.println(testWkMenu.days, BIN);
  
  return testWkMenu.days;
  
}

void testFunct(char param)
{
  if(param)
  {
    Serial.println("Schedule activated");
  }
  else
  {
    Serial.println("Schedule deactivated");
  }
 
}

void updateTime(void * parameters)
{

  for(;;)
  {
    getLocalTime(&timeinfo);
    

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

}


void addSchedule()
{

  clkTime strtTime, endTime;
  //get start time
  strtTime = *getTime();
  //get end time
  endTime = *getTime();
  //get name and days inside function call
  testEvents.addEvent(getStr(), strtTime, endTime, getWeek(), &testFunct);

  mainMenu.addNode(testEvents.events[testEvents.numConfigd-1].name, ACT_NODE, &viewSched);

  test_display.clearDisplay();
  test_display.setCursor(0,0);
  test_display.setTextSize(2);

  test_display.write(" Schedule\n  Added");

  test_display.display();
  
  delay(2000);

  testStick.update();
  
}

void viewSched()
{
  Serial.println(mainMenu.nodeIndex, DEC);
  char schedIndex = mainMenu.nodeIndex;
  char cont = 1;

  Serial.print("Name: ");
  Serial.println(testEvents.events[schedIndex].name);

  Serial.print("Start: ");
  Serial.print(testEvents.events[schedIndex].startTime.hr, DEC);
  Serial.println(testEvents.events[schedIndex].startTime.min, DEC);

  Serial.print("End: ");
  Serial.print(testEvents.events[schedIndex].endTime.hr, DEC);
  Serial.println(testEvents.events[schedIndex].endTime.min, DEC);

  
    delay(5000);
}

void scanWifi()
{
    //scan for networks

    //create menu to navigate networks?

    //config new wifi network

    //input password based on network name

    //$$$
}
