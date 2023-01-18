#include <Simple_Menu.h>

#include <Arduino.h>
menuFrame mainMenu;

void setup() 
{

Serial.begin(115200);

oledSystemInit();

  //Main menu
  mainMenu.addMenu("Main Menu", 0);
  mainMenu.addNode("Heating", SUB_NODE, NULL);
  mainMenu.linkNode(1);
  
  mainMenu.addNode("RF Devices", SUB_NODE, NULL);
  mainMenu.linkNode(2);


  mainMenu.addNode("IR Devices", SUB_NODE, NULL);
  mainMenu.linkNode(3);


  mainMenu.addNode("Wifi Settings", SUB_NODE, NULL);
  mainMenu.linkNode(4);


  mainMenu.addNode("LCD Settings", SUB_NODE, NULL);
  mainMenu.linkNode(5);
  

  //Submenu 1
  mainMenu.addMenu("Heating", 1);
  mainMenu.addNode("Quick Heat", ACT_NODE, &testFunct);
  
  mainMenu.addNode("Scheduling", SUB_NODE, NULL);
  mainMenu.linkNode(7);
  
  
  mainMenu.addNode("Turn Off", ACT_NODE, &testFunct);

  //Submenu 2
  mainMenu.addMenu("RF Devices", 2);
  mainMenu.addNode("Scheduling", SUB_NODE, NULL);
  mainMenu.linkNode(8);
  
  mainMenu.addNode("Add Device", ACT_NODE, &testFunct);
  mainMenu.addNode("Device List", ACT_NODE, &testFunct);


  //Submenu 3
  mainMenu.addMenu("IR Devices", 3);
  mainMenu.addNode("Scheduling", SUB_NODE, NULL);
  mainMenu.linkNode(9);
  
  mainMenu.addNode("Add Device", ACT_NODE, &testFunct);
  mainMenu.addNode("Device List", ACT_NODE, &testFunct);

  
  //Submenu 4
  mainMenu.addMenu("Wifi", 4);
  mainMenu.addNode("Check IP", ACT_NODE, &testFunct);
  mainMenu.addNode("Scan Networks", ACT_NODE, &testFunct);
  mainMenu.addNode("Connect", ACT_NODE, &testFunct);
  mainMenu.addNode("Disconnect", ACT_NODE, &testFunct);
  mainMenu.addNode("Device List", ACT_NODE, &testFunct);

  //Submenu 5
  mainMenu.addMenu("LCD Settings", 5);
  mainMenu.addNode("Backlight Toggle", ACT_NODE, &testFunct);

  //Submenu 7
  mainMenu.addMenu("Heat Sched.", 7);
  mainMenu.addNode("Add Schedule", ACT_NODE, &testFunct);
  mainMenu.addNode("Change Schedule", ACT_NODE, &testFunct);
  mainMenu.addNode("Remove Schedule", ACT_NODE, &testFunct);

  //Submenu 8
  mainMenu.addMenu("RF Sched.", 8);
  mainMenu.addNode("Add Schedule", ACT_NODE, &testFunct);
  mainMenu.addNode("Change Schedule", ACT_NODE, &testFunct);
  mainMenu.addNode("Remove Schedule", ACT_NODE, &testFunct);

  //Submenu 9
  mainMenu.addMenu("IR Sched.", 9);
  mainMenu.addNode("Add Schedule", ACT_NODE, &testFunct);
  mainMenu.addNode("Change Schedule", ACT_NODE, &testFunct);
  mainMenu.addNode("Remove Schedule", ACT_NODE, &testFunct);
}

void loop() 
{
  mainMenu.build();

  if(Serial.available())
  {
    switch(Serial.read())
    {
      case 'u':
        mainMenu.up();
      break;
        
      case 'd':
        mainMenu.down();
      break;
  
      case 'c':
        mainMenu.choose();
      break;
  
      case 'b':
      //Need to backlink nodes to menus with another variable "linkedNode"
        mainMenu.back();
      break;
  
      default:
        
      break;
    }
  }
  
  delay(100);
  
}

void testFunct()
{
  Serial.println("Function successfully called");
}
