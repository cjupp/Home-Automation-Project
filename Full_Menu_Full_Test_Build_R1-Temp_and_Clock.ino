/* -------------------------------------------------------------------------------------------------------------------- |
| --------------------------------  / \  / /  / __  /  /__ __/  / ___/  / ____/ --------------------------------------- |
| -------------------------------  / /\\/ /  / /_/ /    / /    / __/    \__ \  ---------------------------------------- |
| ------------------------------  /_/  \_/  /_____/    /_/    /____/  /_____/ ----------------------------------------- |
| --------------------------------------------------------------------------------------------------------------------- |
| Last left off at:
|
| - make oled hmi functions into threads with temporary objects
|
| - generalize menu and other higher level functions
|
| - add option into Simple Menu library to add nodes to specific menu
|
| - add delete schedule function (remove menu node, reset event)
|
| - Screen timeout after 15 seconds
|
| - Move everything to library
|
| - make task for displaying to oled and other serial functions highest priority? (Trying to avoid screen tearing)
|
| - add list object for oled_hmi library
|
|
| --------------------------------------------------------------------------------------------------------------------- */

#include <Arduino.h>

#include <WiFi.h>
#include "time.h"
#include "sntp.h"

#include <Wire.h>

#include "Arduino.h"

#include "Active_WiFi.h"
#include "Analog_Stick.h"
#include "EEPROM_Manager.h"
#include "Events_Scheduling.h"
#include "HTTP_Server.h"
#include "OLED_Tools.h"
#include "OLED_HMI.h"
#include "Serial_Tools.h"
#include "Simple_Menu_Scrolling.h"
#include "Real_Timekeeping.h"
#include "Heating.h"


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

//#include "Home_Automation.h"

// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------- /    \   / ___/  / ___/  /_  _/  / \  / /  / ___/  / ____/ ----------------------------- //
// -------------------------- / /_\ /  / __/   / __/     / /   / /\\/ /  / __/    \__ \  ------------------------------ //
// ------------------------- /_____/  /____/  /_/      /___/  /_/  \_/  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// Define the core that the RTOS is running on
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------- ANALOG STICK ---------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //

#define BPIN  33
#define XPIN  35
#define YPIN  34

#define HEAT_PIN 2

#define MAIN_MENU_DELAY 100

// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------ IPS LCD ------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //
#define TFT_CS         5
#define TFT_RST        16
#define TFT_DC         15

#define TFT_MOSI      13
#define TFT_SCLK      14




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

strInput strMenu;

weekdayInput weekMenu;

timeInput timeMenu;

yesNoInput ynMenu;

heater HomeHeater(HEAT_PIN);

// OR for the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


Adafruit_AM2320 am2320 = Adafruit_AM2320();

char lcdBuff[30];

char weekdays[7][5] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};


// Declare global variables
float temp = 0;
float hum = 0;
char buffer[2][16];


const char* ssid       = "Sah Gah Wah-Fah";
const char* password   = "3FrahndsSAHGAH";

//const char* ssid       = "Smitty Werbenjaegermanjensen";
//const char* password   = "Hewasnumber1";


// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

char HA_SystemInit()
{
  Serial.begin(115200);

  pinMode(2, OUTPUT);

  // Initialize and clear the OLED display
  oledSystemInit();

  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE);
  oledDisplay.setCursor(0,0);
  oledDisplay.setTextSize(2);

  oledDisplay.write("Starting");

  oledDisplay.display();

  delay(2000);
  
  oledDisplay.clearDisplay();

  oledDisplay.display();
  
  //wifiSystemInit();

  // Initialize and clear the LCD display
  //lcdSystemInit();


    // Call initializations for all libraries here

    // return -1 if something does not initialize properly

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  timeSystemInit();

}

// Request the information from our sensor
void getData()
{
  temp = am2320.readTemperature();
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  hum = am2320.readHumidity();
  vTaskDelay(5000 / portTICK_PERIOD_MS);
}

// Display the information to our LCD and the terminal
void dispData()
{
  sprintf(buffer[0], "Temp: %.2f C \n", temp);
  sprintf(buffer[1], "Hum:  %.2f %% ", hum);

  //vTaskDelay(5000 / portTICK_PERIOD_MS);
  
  //IPS LCD display
  tft.fillScreen(ST77XX_BLACK);
  //tft.setTextSize(3);
  //tft.setCursor(0, 0);
  //tft.setTextColor(ST77XX_BLACK);
  //tft.print("                \n                ");

  

  sprintf(lcdBuff, "%s,\n%s %d\n%d\n%02d:%02d", weekdays[esp32Time.tm_wday], months[esp32Time.tm_mon], esp32Time.tm_mday, (esp32Time.tm_year + 1900), esp32Time.tm_hour, esp32Time.tm_min);
  
  //vTaskDelay(5000 / portTICK_PERIOD_MS);
  
  tft.setTextSize(3);
  tft.setCursor(0, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(lcdBuff);

  //vTaskDelay(5000 / portTICK_PERIOD_MS);
  
  tft.setTextSize(3);
  tft.setCursor(0, 150);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextWrap(true);
  tft.print(buffer[0]);
  tft.print(buffer[1]);
  

}

void dataCycle(void * parameters)
{
  for(;;)
  {
    getData();
    dispData();
    vTaskDelay(20000 / portTICK_PERIOD_MS);
  }

}

void checkHeatTask(void * parameters)
{
  for(;;)
  {
    checkSched(&heatEvents);
    vTaskDelay(30000 / portTICK_PERIOD_MS);
  }
}

void checkSched(eventHandler * schedules)
{
  Serial.println("Checking Events...");
  Serial.println(esp32Time.tm_hour, DEC);
  Serial.println(esp32Time.tm_min, DEC);
  schedules->check(esp32Time.tm_hour, esp32Time.tm_min, 0b00010000);
}

void menuThread(void * parameters)
{
  for(;;)
  {
      menuLoop(&mainMenu, &joystick);
      vTaskDelay(MAIN_MENU_DELAY / portTICK_PERIOD_MS);
  }
}

void menuLoop(menuFrame * menu, analogStick * stick)
{
  menu->newBuild();
  stick->update();
   if(!stick->held.x)
   {
     switch (stick->status.x)
     {
      case 'l':
      menu->back();
      break;
  
      case 'r':
      menu->choose();
      break;
     }
   }
   if(!stick->held.y)
   {
     switch (stick->status.y)
     {
      case 'u':
      menu->newUp();
      break;
  
      case 'd':
      menu->newDown();
      break;
     }
   }
   if(!stick->held.button && stick->status.button)
   {
      menu->choose();
   }
}

char getStr(char * string)
{
  strMenu.reset();
  char continuous = 0;
  char returnVal;
  
  do
  {
   strMenu.build();
   joystick.update();
   if(!joystick.held.x || continuous)
   {
     switch (joystick.status.x)
     {
      case 'l':
      strMenu.move('l');
      break;
  
      case 'r':
      strMenu.move('r');
      break;
     }
   }
   if(!joystick.held.y || continuous)
   {
     switch (joystick.status.y)
     {
      case 'u':
      strMenu.move('u');
      break;
  
      case 'd':
      strMenu.move('d');
      break;
     }
   }
   if(!joystick.held.button && joystick.status.button)
   {
      strMenu.sel();
   }

   delay(100);
  }while(!strMenu.complete);

  if(strMenu.complete == 1)
  {
    strcpy(string, strMenu.output);
    
    Serial.print("Name: ");
    Serial.println(strMenu.output);
  }
  else
  {
    Serial.println("Canceled");

  }
  return strMenu.complete;
}

 char getTime(clkTime * newTime)
{
  timeMenu.reset();
  char returnVal;
  do
  {
    timeMenu.build();
    joystick.update();
   if(!joystick.held.x)
   {
     switch (joystick.status.x)
     {
      case 'l':
      timeMenu.left();
      break;
  
      case 'r':
      timeMenu.right();
      break;
     }
   }
   if(!joystick.held.y)
   {
     switch (joystick.status.y)
     {
      case 'u':
      timeMenu.up();
      break;
  
      case 'd':
      timeMenu.down();
      break;
     }
   }
   if(!joystick.held.button && joystick.status.button)
   {
      timeMenu.sel();
   }
   delay(100);
    
  }while(!timeMenu.complete);

  if(timeMenu.complete == 1)
  {
    Serial.print("Time: ");
    Serial.print(timeMenu.output[0], DEC);
    Serial.print(timeMenu.output[1], DEC);
    Serial.print(":");
    Serial.print(timeMenu.output[2], DEC);
    Serial.println(timeMenu.output[3], DEC);

    *newTime = *timeMenu.accept();
    
  }
  else
  {
    Serial.println("Canceled");
  }
   return timeMenu.complete;

}

char getWeek(char * weekdays)
{
  weekMenu.reset();
  char returnVal;
  do
  {
    weekMenu.build();
    joystick.update();
    if(!joystick.held.x)
    {
     switch (joystick.status.x)
     {
      case 'l':
      weekMenu.left();
      break;
    
      case 'r':
      weekMenu.right();
      break;
     }
    }
    if(!joystick.held.button && joystick.status.button)
    {
      weekMenu.sel();
    }
   
    delay(100);
    
  }while(!weekMenu.complete);

  Serial.print("Week Returned: ");
  Serial.println(weekMenu.complete, DEC);

  if(weekMenu.complete == 1)
  {
    Serial.print("Weekdays: ");
    Serial.println(weekMenu.days, BIN);
    *weekdays = weekMenu.days;
  }
  else
  {
    Serial.println("Canceled");
  }  
  return weekMenu.complete;
}



char getYN(char * msg)
{
  ynMenu.setMsg(msg);
  char returnVal;
  do
  {
    ynMenu.build();
    joystick.update();
    if(!joystick.held.x)
    {
     switch (joystick.status.x)
     {
      case 'l':
      ynMenu.left();
      break;
    
      case 'r':
      ynMenu.right();
      break;
     }
    }
    if(!joystick.held.button && joystick.status.button)
    {
      returnVal = ynMenu.sel();
    }
   
    delay(100);
    
  }while(!returnVal);

  Serial.print("Answer: ");
  
  if(ynMenu.complete == 1)
  {

    Serial.println("Yes/OK");
  }
  else
  {
    Serial.println("No/Cancel");
  }  
  return ynMenu.complete;
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
    getLocalTime(&esp32Time);
    

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


void toggleHeat(char onOff)
{
  HomeHeater.toggle(onOff);
}


char addSchedule()
{
  clkTime strtTime, endTime;
  char strBuffer[OLED_INPUT_CHARS];
  char weekdays = 0;
  signed char returnVal;
  
  char i = 0;
  do
  {
    switch(i)
    {
      case 0:
        //get start time
        timeMenu.setHeader("Start Time");
        returnVal = getTime(&strtTime);
      break;

      case 1:
        //get end time
        timeMenu.setHeader("End Time");
        returnVal = getTime(&endTime);
      break;

      case 2:
        returnVal = getStr(strBuffer);
      break;

      case 3:
        returnVal = getWeek(&weekdays);
      break;
    }
    Serial.print("Returned: ");
    Serial.println(returnVal, DEC);
  }while(returnVal!= -1 && i++ < 4);

  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE);
  oledDisplay.setCursor(0,0);
  oledDisplay.setTextSize(2);
  
  if(returnVal == 1)
  {
    // Add the event to the eventHandler
    heatEvents.addEvent(strBuffer, strtTime, endTime, weekdays, &toggleHeat);
    
    // Add the node to the menu to be viewed
    mainMenu.manAddNode(heatEvents.events[heatEvents.numConfigd-1].name, 4, ACT_NODE, &viewSched);

    // Add the node to the delete menu
    mainMenu.manAddNode(heatEvents.events[heatEvents.numConfigd-1].name, 5, ACT_NODE, &viewSched);
    
    oledDisplay.write(" Schedule\n  Added");
  }
  else
  {
    oledDisplay.write(" Canceled");
  }
  
  oledDisplay.display();  
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  joystick.update();
}

char viewSched()
{
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE);
  oledDisplay.setCursor(0,0);
  
  
  char schedIndex = mainMenu.nodeIndex;

  Serial.println(heatEvents.events[schedIndex].name);
  Serial.println(heatEvents.events[schedIndex].startTime.hr, DEC);
  Serial.println(heatEvents.events[schedIndex].startTime.min, DEC);
  Serial.println(heatEvents.events[schedIndex].endTime.hr, DEC);
  Serial.println(heatEvents.events[schedIndex].endTime.min, DEC);

  oledDisplay.setTextSize(2);
  oledDisplay.printf("%s\n", heatEvents.events[schedIndex].name);

  oledDisplay.setTextSize(1);
  oledDisplay.printf("Start:\n%u:%u\n", heatEvents.events[schedIndex].startTime.hr, heatEvents.events[schedIndex].startTime.min);

  oledDisplay.printf("End:\n%u:%u\n", heatEvents.events[schedIndex].endTime.hr, heatEvents.events[schedIndex].endTime.min);

  char days[25];
  char daysIndex = 0;

  for(int i = 0; i < 8; i++)
  {
    if(BIT0<<i & heatEvents.events[schedIndex].weekDays)
    {
      days[daysIndex++] = hmiWeekdays[i][0];
      days[daysIndex++] = hmiWeekdays[i][1];
      days[daysIndex++] = ' ';   
    }
  }
  days[daysIndex] = '\0';

  oledDisplay.printf("Days:\n%s\n", days);

  oledDisplay.display();
  vTaskDelay(5000 / portTICK_PERIOD_MS);
}

void deleteSched()
{
  //create delete method in Simple Menu

  //remove menu item based on menu index

  //delete schedule from array
}


void startCheck()
{
  // Load in saved wifi network(s) and schedules
  // If there is a valid wifi connection saved, attempt to connect to it
  // If there is no valid network, prompt the user if they want to add a network
  
  // If the connection fails, prompt user to choose if they want to add a new network
  // If they choose no, then prompt for the time
}



void addNetwork()
{
  // use passed in network name
  // ask for password
  // create new network with them
}

void manSetTime()
{
  // get time with heading "Set Time"
  // set internal clock
  
}



void askWiFi()
{
  // create yes/no prompt in OLED_HMI library

  // if yes, scan wifi, input password with getStr and connect

  // if no, input current time thorugh getTime 
  
}

void startCheckup()
{
  // check for schedules and internet connections in eeprom

  // if there is a wifi network, attempt connection

  // if connection fails or there is no saved connection, ask if user wants to connect

  // otherwise get NTP
}


void setup() 
{
  HA_SystemInit();
  
  am2320.begin();

  // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320, 2);           // Init ST7789 320x240

  tft.setRotation(1);
  
  tft.fillScreen(ST77XX_BLACK);
  
  
  

  //Main menu
  mainMenu.addMenu("Main Menu", 0);

  mainMenu.addNode("Heating", SUB_NODE, NULL);
  mainMenu.linkNode(1);

  mainMenu.addNode("WiFi", SUB_NODE, NULL);
  mainMenu.linkNode(2);

  mainMenu.addNode("Time", SUB_NODE, NULL);
  mainMenu.linkNode(3);


  mainMenu.addMenu("Heating", 1);
  
  mainMenu.addNode("Add", ACT_NODE, &addSchedule);

  mainMenu.addNode("Delete", SUB_NODE, NULL);
  mainMenu.linkNode(5);

  mainMenu.addNode("View", SUB_NODE, NULL);
  mainMenu.linkNode(4);
  

  mainMenu.addMenu("WiFi", 2);

  mainMenu.addNode("Scan WiFi", ACT_NODE, NULL);

  mainMenu.addNode("Disconnect", ACT_NODE, NULL);

  mainMenu.addNode("Reconnect", ACT_NODE, NULL);


  mainMenu.addMenu("Time", 3);

  mainMenu.addNode("Set Time", ACT_NODE, NULL);

  mainMenu.addNode("Get NTP", ACT_NODE, NULL);
  

  mainMenu.addMenu("View", 4);
  //Menu nodes added by other function


  mainMenu.addMenu("Delete", 5);
  //Menu nodes added by other function




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
  checkHeatTask,
  "Events Check",
  2000,
  NULL,
  10,
  NULL,
  ARDUINO_RUNNING_CORE
  );

  // Schedule Checker
  xTaskCreatePinnedToCore(
  menuThread,
  "HMI",
  2000,
  NULL,
  8,
  NULL,
  ARDUINO_RUNNING_CORE
  );

    // Schedule Checker
  xTaskCreatePinnedToCore(
  dataCycle,
  "Temp Read",
  10000,
  NULL,
  12,
  NULL,
  ARDUINO_RUNNING_CORE
  );
  

}

void loop() 
{
  
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
