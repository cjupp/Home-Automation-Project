#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Serial_Tools.h>
#include <Analog_Stick.h>
#include <Simple_Menu.h>
#include <OLED_HMI.h>
#include <OLED_Tools.h>
#include <Events_Scheduling.h>

#include <WiFi.h>
#include "time.h"
#include "sntp.h"
//#include <events_scheduling.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif



//Wire.begin(24, 21);
Adafruit_SSD1306 test_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define BPIN  33
#define XPIN  34
#define YPIN  35

analogStick testStick(XPIN, YPIN, BPIN, 2000);

menuFrame mainMenu;

eventHandler testEvents;

strInput testStrMenu;
timeInput testClkMenu;
weekdayInput testWkMenu;


long startTime = 0;

//const char* ssid       = "TELUS2728";
//const char* password   = "kvy57kxfdq";

const char* ssid       = "Sah Gah Wah-Fah";
const char* password   = "3FrahndsSAHGAH";

//const char* ssid       = "CrowsNet";
//const char* password   = "G00dD0gski";

//const char* ssid       = "Smitty Werbenjaegermanjensen";
//const char* password   = "Hewasnumber1";


const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = -8*3600;
const int   daylightOffset_sec = 3600;

const char* time_zone = "PST8PDT,M3.2.0,M11.1.0";  

char * printfPtr;

struct tm timeinfo;

char gotTime = 0;

char oledBuff[50];

char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void printTime(void * parameters)
{

  for(;;)
  {
    getLocalTime(&timeinfo);
  
    sprintf(oledBuff, "%s,\n%s %d\n%d\n%02d:%02d:%02d", weekdays[timeinfo.tm_wday], months[timeinfo.tm_mon], timeinfo.tm_mday, (timeinfo.tm_year + 1900), timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    test_display.clearDisplay();
    test_display.setCursor(0,0);
    test_display.setTextColor(WHITE); // Draw white text
    test_display.setTextSize(2);      // Normal 1:1 pixel scale
    
    test_display.write(oledBuff);
  
    test_display.display();

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
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


void setup() 
{
  Serial.begin(115200);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!test_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  test_display.cp437(true);         // Use full 256 char 'Code Page 437' font

  test_display.clearDisplay();

  test_display.setCursor(0,0);
  test_display.setTextSize(2);

  test_display.write("Initializing");

  test_display.display();

  delay(2000);

  test_display.clearDisplay();

  test_display.display();

    // set notification call-back function
  sntp_set_time_sync_notification_cb( timeavailable );

  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  //sntp_servermode_dhcp(1);    // (optional)

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /**
   * A more convenient approach to handle TimeZones with daylightOffset 
   * would be to specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  //configTzTime(time_zone, ntpServer1, ntpServer2);

  //connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("Connected!");

  while(!getLocalTime(&timeinfo))
  {
    Serial.println("No time available (yet)");
    delay(5000);
  }

  xTaskCreatePinnedToCore(
  updateTime,
  "Print Time",
  2000,
  NULL,
  9,
  NULL,
  ARDUINO_RUNNING_CORE
  );

  // Schedule Checker
  xTaskCreatePinnedToCore(
  checkSched,
  "Events Check",
  2000,
  NULL,
  10,
  NULL,
  ARDUINO_RUNNING_CORE
  );

    


//-------------------------------------------------------------------------------
hmiSystemInit();
oledSystemInit();

  //Main menu
  mainMenu.addMenu("Main Menu", 0);
  
  mainMenu.addNode("Add Schedule", ACT_NODE, &addSchedule);

  mainMenu.addNode("View Schedules", SUB_NODE, NULL);
  mainMenu.linkNode(2);

  mainMenu.addMenu("View", 2);

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
   delay(100);
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
| - use one universal display object of each type that only gets used temporarily then deleted
|
| - save schedules to flash
|
| - add titles to clock hmi and weekday hmi
|
| - add wifi option on startup, input time if no
|
| --------------------------------------------------------------------------------------------------------------------- */ 
