/*--------------------------------------------------------------------------------------------------------------------
| analog_stick.h: 
|
| Created by:     Cameron Jupp
| Date Started:   December 30, 2022
--------------------------------------------------------------------------------------------------------------------*/ 

/* -------------------------------------------------------------------------------------------------------------------- |
| --------------------------------  / \  / /  / __  /  /__ __/  / ___/  / ____/ --------------------------------------- |
| -------------------------------  / /\\/ /  / /_/ /    / /    / __/    \__ \  ---------------------------------------- |
| ------------------------------  /_/  \_/  /_____/    /_/    /____/  /_____/ ----------------------------------------- |
| --------------------------------------------------------------------------------------------------------------------- |
| Last left off at:
|
| - Possibly simplify analog output to a single direction in the future?
|
|
|
| --------------------------------------------------------------------------------------------------------------------- */ 



// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------- /    \   / ___/  / ___/  /_  _/  / \  / /  / ___/  / ____/ ----------------------------- //
// -------------------------- / /_\ /  / __/   / __/     / /   / /\\/ /  / __/    \__ \  ------------------------------ //
// ------------------------- /_____/  /____/  /_/      /___/  /_/  \_/  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //
#define ADC_RES 4095

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------- / ___/  / /     / __  /  / _ /    / _\      / /     / ____/ ----------------------------- //
// ------------------------- / /_//  / /__   / /_/ /  / _  \   / /_\\    / /__    \__ \  ------------------------------ //
// ------------------------ /____/  /____/  /_____/  /_____/  /_/   \\  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------- / ___/  / /     / _\      / ____/  / ____/  / ___/  / ____/ -------------------------- //
// ---------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  --------------------------- //
// --------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ---------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //


/*-----------------------------------------------------------------------------------------------------------------------/ 
|------------------------------------------------------ CLASS ----------------------------------------------------------/
|----------------------------------------------------------------------------------------------------------------------/
| analogInfo: a simple struct to hold the status of an analog stick
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
|-----------------------------------------------------------------------------------------------------------------------/
|----------------------------------------------------------------------------------------------------------------------/
|--------------------------------------------------------------------------------------------------------------------*/
struct analogInfo
{
  char button;

  char x;

  char y;
};


/*-----------------------------------------------------------------------------------------------------------------------/ 
|------------------------------------------------------ CLASS ----------------------------------------------------------/
|----------------------------------------------------------------------------------------------------------------------/
| analogStick: an object to hold the status of, and to update a physical analog stick
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
|-----------------------------------------------------------------------------------------------------------------------/
|----------------------------------------------------------------------------------------------------------------------/
|--------------------------------------------------------------------------------------------------------------------*/
class analogStick
{
    public:

    int xV = 0;
    int yV = 0;

    analogInfo pins;

    analogInfo status;

    analogInfo lastStatus;

    analogInfo held;

    int threshold;

    // Class constructor
    analogStick(char xPin, char yPin, char butPin, int thresh);
    void getX();
    void getY();
    char getDirX();
    char getDirY();
    char getButton();
    void update();
};


// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

//void executeX(analogStick * stick, char continuous, void (*functL) (void), void (*functR) (void));

//void executeY(analogStick * stick, char continuous, void (*functU) (void), void (*functD) (void));

//void executeB(analogStick * stick, void (*funct) (void));
