#include <Arduino.h>
#include "Simple_Menu_Scrolling.h"
#include "Analog_Stick.h"
#include "OLED_Tools.h"
#include "OLED_HMI.h"
#include <WiFi.h>

#define BPIN  33
#define XPIN  34
#define YPIN  35

char nameBuff[40];
char passBuff[40];
menuFrame mainMenu;
menuFrame wifiList;
strInput strMenu;

// Declare the analog stick used for the project using the defined pins and threshold
analogStick joystick(XPIN, YPIN, BPIN, 2000);


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


void scanWiFi()
{
  int netNum = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  netNum = WiFi.scanNetworks();

  for(int i = 0; i < netNum; i++)
  {
    Serial.println(WiFi.SSID(i));
    WiFi.SSID(i).toCharArray(nameBuff, WiFi.SSID(i).length()+1);
    wifiList.addNode(nameBuff, ACT_NODE, &connectWiFi);
  }

  for(;;)
  {
    menuLoop(&wifiList, &joystick);
  }

  
  
    //scan for networks

    //create list object with networks

    //call addNetwork function with chosen item's name string

    //$$$
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


void connectWiFi()
{
  getStr(passBuff);
  WiFi.SSID(wifiList.nodeIndex).toCharArray(nameBuff, WiFi.SSID(wifiList.nodeIndex).length()+1);
  Serial.println(nameBuff);
  Serial.println(passBuff);
  
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE); // Draw white text
  oledDisplay.setCursor(0, 0);     // Start at top-left corner
  
  oledDisplay.printf("Connecting to\n%s", nameBuff);
  oledDisplay.display();
  
  WiFi.begin(nameBuff, passBuff);
  while (WiFi.status() != WL_CONNECTED) 
  {
  delay(500);
  Serial.print(".");
  }
  Serial.println("Connected!");
}

void setup() 
{
  Serial.begin(115200);
  
  // Initialize and clear the OLED display
  oledSystemInit();
  
  
  wifiList.addMenu(" APs", 0);
}

void loop() 
{
  scanWiFi();
}
