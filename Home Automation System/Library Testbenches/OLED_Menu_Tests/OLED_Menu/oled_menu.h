

#define MAX_CHAR        16
#define MAX_NODES       6
#define MAX_MENUS       10

#define SUB_MENU        1
#define ACT_MENU        2

#define MENU            0
#define NODE            1


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

    void config(char * namePtr, char type, void (*functPtr) (void));
    void setName(char * namePtr);
    void link(char menuNum);

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

    void addNode(char * nodeName, char nodeType, void (*functPtr) (void));
    void void setName(char * namePtr);
    void build(char index);

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

    void build();
    void addMenu();
    void addNode();
    void linkNode();
    void up();
    void down();
    void choose();
    void back();
};

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //
