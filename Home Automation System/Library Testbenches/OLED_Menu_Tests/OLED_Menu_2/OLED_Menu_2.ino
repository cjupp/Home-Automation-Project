
#include <Wire.h>
#include <Adafruit_SSD1306.h>
//#include <Arduino.h>

// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------- /    \   / ___/  / ___/  /_  _/  / \  / /  / ___/  / ____/ ----------------------------- //
// -------------------------- / /_\ /  / __/   / __/     / /   / /\\/ /  / __/    \__ \  ------------------------------ //
// ------------------------- /_____/  /____/  /_/      /___/  /_/  \_/  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

#define MAX_CHAR        16
#define MAX_NODES       6
#define MAX_MENUS       10

#define SUB_MENU        1
#define ACT_MENU        2

#define MENU            0
#define NODE            1

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------- / ___/  / /     / _\      / ____/  / ____/  / ___/  / ____/ -------------------------- //
// ---------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  --------------------------- //
// --------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ---------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// Holds each individual node name and function
class menuNode
{
// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------- PUBLIC ------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //
  public:
  char name[MAX_CHAR];

  // Tells if a node triggers an action or opens another menu
  char nodeType = 0;

  // Stores the menu that the node might call if it is configured as a submenu node
  char menuCall = 0;

  // The function pointer for the node if is configured as an action node
  void (*choose)(void);

  void config(char * namePtr, char type, void (*functPtr) (void))
  {
    this->nodeType = type;

    this->setName(namePtr);    

    this->choose = functPtr;


  }

  void setName(char * namePtr)
  {
    strcpy(this->name, namePtr);
  }

  void link(char menuNum)
  {
    this->menuCall = menuNum;
  }

  

};


// Keeps track of the nodes in each menu and holds the information together for 
// organizational purposes
class menu
{
  public:
  char name[MAX_CHAR];

  menuNode node[MAX_NODES];

  char nodeCount = 0;

  char backLink[2];

  void addNode(char * nodeName, char nodeType, void (*functPtr) (void))
  {
    node[nodeCount].config(nodeName, nodeType, functPtr);

    nodeCount++;
    
  }
  

  void setName(char * namePtr)
  {
    strcpy(this->name, namePtr);
  }

  // Displays the contents of a menu
  void build(char index)
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
        
        //Serial.println(this->node[i].name);
      }
    }

    display.display();
  }

};

// Holds all of the menus and keeps track of user input and indexing
// also builds/displays the menus
class menuFrame
{
  public:
  // Keeps track of the up/down position on the menu
  menu menuList[MAX_MENUS];

  char choiceIndex = 0;
  
  char currentMenu = 0;
  char configIndex = 0;
  char nodeLinkIndex = 0;

  void build()
  {
    menuList[currentMenu].build(this->choiceIndex);
  }

  void addMenu (char * menuName, char index)
  {
    menuList[index].setName(menuName);

    configIndex = index;
  }

  void addNode(char * nodeName, char nodeType, void (*functPtr) (void))
  {
    this->menuList[configIndex].addNode(nodeName, nodeType, functPtr);

    this->nodeLinkIndex = this->menuList[configIndex].nodeCount - 1;
  }

  void linkNode(char menuNum)
  {
    // Add the menu a node directs to
    this->menuList[configIndex].node[nodeLinkIndex].link(menuNum);

    // Create a back reference to the parent node of a menu
    this->menuList[menuNum].backLink[MENU] = this->configIndex;
    this->menuList[menuNum].backLink[NODE] = this->nodeLinkIndex;
  }

  
  void up()
  {
    // If the index is greater than zero, decrement it (top choice is zero)
    if(choiceIndex)
    {
    this->choiceIndex--;
    }
  }

  void down()
  {
    // If the index is less than the menu size, then increment the index (last option is largest value)
    if(choiceIndex < menuList[currentMenu].nodeCount - 1)
    {
    this->choiceIndex++;
    }
  }

  void choose()
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

  void back()
  {
    this->choiceIndex = this->menuList[currentMenu].backLink[NODE];
    this->currentMenu = this->menuList[currentMenu].backLink[MENU];
  }
  
};

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //


menuFrame mainMenu;



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

  
  //Main menu
  mainMenu.addMenu("Menu 1", 0);
  mainMenu.addNode("Menu 1 Node 1", SUB_MENU, NULL);
  mainMenu.linkNode(1);
  
  mainMenu.addNode("Menu 1 Node 2", SUB_MENU, NULL);
  mainMenu.linkNode(2);


  mainMenu.addNode("Menu 1 Node 3", SUB_MENU, NULL);
  mainMenu.linkNode(3);


  //Submenu 1
  mainMenu.addMenu("SubM 1", 1);
  mainMenu.addNode("SubM1 Node 1", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM1 Node 2", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM1 Node 3", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM1 Node 4", ACT_MENU, &testFunct);

  //Submenu 2
  mainMenu.addMenu("SubM 2", 2);
  mainMenu.addNode("SubM2 Node 1", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM2 Node 2", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM2 Node 3", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM2 Node 4", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM2 Node 5", ACT_MENU, &testFunct);

  //Submenu 3
  mainMenu.addMenu("SubM 3", 3);
  mainMenu.addNode("SubM3 Node 1", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM3 Node 2", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM3 Node 3", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM3 Node 4", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM3 Node 5", ACT_MENU, &testFunct);
  mainMenu.addNode("SubM3 Node 6", ACT_MENU, &testFunct);


  
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
