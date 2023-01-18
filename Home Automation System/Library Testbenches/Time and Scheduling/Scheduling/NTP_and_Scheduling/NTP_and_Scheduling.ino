#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include <events_scheduling.h>



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif



eventHandler testSched;

long startTime = 0;

//const char* ssid       = "TELUS2728";
//const char* password   = "kvy57kxfdq";

//const char* ssid       = "Sah Gah Wah-Fah";
//const char* password   = "3FrahndsSAHGAH";

const char* ssid       = "CrowsNet";
const char* password   = "G00dD0gski";



const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = -8*3600;
const int   daylightOffset_sec = 3600;

const char* time_zone = "PST8PDT,M3.2.0,M11.1.0";  

char * printfPtr;

struct tm timeinfo;

char gotTime = 0;

char oledBuff[30];

char weekdays[7][5] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};

char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void printTime(void * parameters)
{

  for(;;)
  {
    getLocalTime(&timeinfo);
  
    sprintf(oledBuff, "%s,\n%s %d\n%d\n%02d:%02d:%02d", weekdays[timeinfo.tm_wday], months[timeinfo.tm_mon], timeinfo.tm_mday, (timeinfo.tm_year + 1900), timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE); // Draw white text
    display.setTextSize(2);      // Normal 1:1 pixel scale
    
    display.write(oledBuff);
  
    display.display();

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
    testSched.check(timeinfo.tm_hour, timeinfo.tm_min, 0b00010000);
    vTaskDelay(30000 / portTICK_PERIOD_MS);
  }

}

void testEvent(char onOff)
{
  
  
  if(onOff)
  {
    Serial.println("Event Activated");
  }
  else
  {
    Serial.println("Event Deactivated");
  }
  
}


void setup()
{
  Serial.begin(115200);

  testSched.addEvent("event 0", {16, 35}, {16, 37}, 0b00111110, &testEvent);
  testSched.addEvent("event 1", {16, 36}, {16, 38}, 0b00111110, &testEvent);
  testSched.addEvent("event 2", {16, 37}, {16, 39}, 0b00111110, &testEvent);
  testSched.addEvent("event 3", {16, 38}, {16, 40}, 0b00111110, &testEvent);



    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.clearDisplay();

  display.display();
  
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

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE); // Draw white text
  display.setTextSize(2);      // Normal 1:1 pixel scale

  //connect to WiFi
  display.printf("Connecting to\n%s ", ssid);
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      display.write(".");
      display.display();
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.write(" CONNECTED");
  display.display();

  while(!getLocalTime(&timeinfo))
  {
    Serial.println("No time available (yet)");
    delay(5000);
  }






  xTaskCreatePinnedToCore(
    printTime,
    "Print Time",
    2000,
    NULL,
    9,
    NULL,
    ARDUINO_RUNNING_CORE
    );

    xTaskCreatePinnedToCore(
    checkSched,
    "Events Check",
    2000,
    NULL,
    10,
    NULL,
    ARDUINO_RUNNING_CORE
    );

    

}

void loop()
{


}
