#include <Arduino.h>
#include "Simple_Menu_Scrolling.h"
#include "Analog_Stick.h"
#include "OLED_Tools.h"

#define BPIN  33
#define XPIN  34
#define YPIN  35

char nameBuff[16];
menuFrame mainMenu;

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


void delNode()
{
  mainMenu.delNode(0, mainMenu.nodeIndex);
}


void setup() 
{
  Serial.begin(115200);
  
  // Initialize and clear the OLED display
  oledSystemInit();
  
  mainMenu.addMenu("Main Menu", 0);

for(int i = 0; i < 19; i++)
{
  sprintf(nameBuff, "Node %d", i);

  Serial.println(nameBuff);

  mainMenu.addNode(nameBuff, ACT_NODE, &delNode);
}



}

void loop() 
{
  menuLoop(&mainMenu, &joystick);
}
