/*--------------------------------------------------------------------------------------------------------------------
| HA_Timekeeping.cpp: A mix of NTP time retrieval and general timekeeping. Intended to be used for home automation
|
| Created by:     Cameron Jupp
| Date Started:   January 7, 2023
--------------------------------------------------------------------------------------------------------------------*/

#include <Arduino.h>
#include "OLED_Tools.h"
#include "Real_Timekeeping.h"
#include "time.h"
#include "sntp.h"

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------- / ___/  / /     / __  /  / _ /    / _\      / /     / ____/ ----------------------------- //
// ------------------------- / /_//  / /__   / /_/ /  / _  \   / /_\\    / /__    \__ \  ------------------------------ //
// ------------------------ /____/  /____/  /_____/  /_____/  /_/   \\  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// Associate the months with 3 letter strings using the month number as index
char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char hmiWeekdays[7][3] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

// NTP Time information
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = -8*3600;
const int   daylightOffset_sec = 3600;
const char* time_zone = "PST8PDT,M3.2.0,M11.1.0";  

// Declare structure to hold the time 
struct tm esp32Time;

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
void timeSystemInit()
{
  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

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

  getNTP();
}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  //printLocalTime();
}

void getNTP()
{
  while(!getLocalTime(&esp32Time))
  {
    Serial.println("No time available (yet)");
    delay(5000);
  }
  Serial.println("Got time adjustment from NTP!");

}

void oledPrintTime(void * parameters)
{
    getLocalTime(&esp32Time);
  
    sprintf(oledBuff, "%s,\n%s %d\n%d\n%02d:%02d:%02d", hmiWeekdays[esp32Time.tm_wday], months[esp32Time.tm_mon], esp32Time.tm_mday, (esp32Time.tm_year + 1900), esp32Time.tm_hour, esp32Time.tm_min, esp32Time.tm_sec);
    
    oledDisplay.clearDisplay();
    oledDisplay.setCursor(0,0);
    oledDisplay.setTextColor(WHITE); // Draw white text
    oledDisplay.setTextSize(2);      // Normal 1:1 pixel scale
    
    oledDisplay.write(oledBuff);
  
    oledDisplay.display();
}

void updateTime()
{
  getLocalTime(&esp32Time);
}

 
