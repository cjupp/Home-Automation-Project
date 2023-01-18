#include <Wire.h>
#include <Adafruit_SSD1306.h>
  
#include "oled_menu.h"

// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------- WAP CLASS ------------------------------------------------------ //
// -------------------------------------------------------------------------------------------------------------------- //
void menuNode::config(char * namePtr, char type, void (*functPtr) (void))
{
    this->nodeType = type;

    this->setName(namePtr);    

    this->choose = functPtr; 
}

 void menuNode::setName(char * namePtr)
  {
    strcpy(this->name, namePtr);
  }

  void menuNode::link(char menuNum)
  {
    this->menuCall = menuNum;
  }

// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------- WAP CLASS ------------------------------------------------------ //
// -------------------------------------------------------------------------------------------------------------------- //
void menu::addNode(char * nodeName, char nodeType, void (*functPtr) (void))
{
node[nodeCount].config(nodeName, nodeType, functPtr);

nodeCount++;

}


void menu::setName(char * namePtr)
{
strcpy(this->name, namePtr);
}

// Displays the contents of a menu
void menu::build(char index)
{
display.clearDisplay();
display.setTextColor(WHITE); // Draw white text
display.setCursor(0, 0);     // Start at top-left corner



// Display the menu title
display.setTextSize(2);      // Normal 1:1 pixel scale
display.write("-");
display.write(this->name);
display.write("-\n");


//Serial.print("--- ");
//Serial.print(this->name);
//Serial.println(" ---");

display.setTextSize(1);      // Normal 1:1 pixel scale

for(int i = 0; i < MAX_NODES; i++)
{
    if(this->node[i].nodeType)
    {
    // Display menu name to OLED and console
    if(i == index)
    {
        display.write("->");
        
        //Serial.print("->");
    }
    else
    {
        display.write("  ");
        
        //Serial.print("  ");
    }
    display.write(this->node[i].name);
    display.write("\n");
    }
}
display.display();
}
// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------- WAP CLASS ------------------------------------------------------ //
// -------------------------------------------------------------------------------------------------------------------- //
  void menuFrame::build()
  {
    menuList[currentMenu].build(this->choiceIndex);
  }

  void menuFrame::addMenu (char * menuName, char index)
  {
    menuList[index].setName(menuName);

    configIndex = index;
  }

  void menuFrame::addNode(char * nodeName, char nodeType, void (*functPtr) (void))
  {
    this->menuList[configIndex].addNode(nodeName, nodeType, functPtr);

    this->nodeLinkIndex = this->menuList[configIndex].nodeCount - 1;
  }

  void menuFrame::linkNode(char menuNum)
  {
    // Add the menu a node directs to
    this->menuList[configIndex].node[nodeLinkIndex].link(menuNum);

    // Create a back reference to the parent node of a menu
    this->menuList[menuNum].backLink[MENU] = this->configIndex;
    this->menuList[menuNum].backLink[NODE] = this->nodeLinkIndex;
  }

  
  void menuFrame::up()
  {
    // If the index is greater than zero, decrement it (top choice is zero)
    if(choiceIndex)
    {
    this->choiceIndex--;
    }
  }

  void menuFrame::down()
  {
    // If the index is less than the menu size, then increment the index (last option is largest value)
    if(choiceIndex < menuList[currentMenu].nodeCount - 1)
    {
    this->choiceIndex++;
    }
  }

  void menuFrame::choose()
  {
    switch(this->menuList[currentMenu].node[choiceIndex].nodeType)
    {
      case SUB_MENU:
      // Change the active menu to that of the node's linked menu number
      this->currentMenu = this->menuList[currentMenu].node[choiceIndex].menuCall;
      // Reset the choice index to zero
      this->choiceIndex = 0;
      break;

      case ACT_MENU:
      // Activate the linked function for the node
      this->menuList[currentMenu].node[choiceIndex].choose();
      break;
    }
  }

  void menuFrame::back()
  {
    this->choiceIndex = this->menuList[currentMenu].backLink[NODE];
    this->currentMenu = this->menuList[currentMenu].backLink[MENU];
  }