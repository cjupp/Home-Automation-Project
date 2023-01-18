/*--------------------------------------------------------------------------------------------------------------------
| analog_stick.cpp: 
|
| Created by:     Cameron Jupp
| Date Started:   December 30, 2022
--------------------------------------------------------------------------------------------------------------------*/ 

#include "Analog_Stick.h"


// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------- / ___/  / /     / _\      / ____/  / ____/  / ___/  / ____/ -------------------------- //
// ---------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  --------------------------- //
// --------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ---------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //


// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------- ANALOGSTICK ---------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //


/*---------------------------------------------------------------------------------------------------------------------
| analogStick: The constructor for the analog stick object; inputs the used pins and commences required 
| initializations. The threshold for the analog stick directions is also input here
|  
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - xPin: The pin number for the x analog input
|
| - yPin: The pin number for the y analog input
|
| - butPin: The pin for monitoring the push button on the analog stick
|
| - thresh: The threshold from the neutral position position of the analog stick that will register the stick as going
| in a certain direction. Units are ADC measurments
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
analogStick::analogStick(char xPin, char yPin, char buttonPin, int thresh)
{
    // Initialize button pin as input
    pinMode(buttonPin, INPUT);

    this->pins.x = xPin;
    this->pins.y = yPin;
    this->pins.button = buttonPin;

    this->threshold = thresh;
}


/*---------------------------------------------------------------------------------------------------------------------
| getX: Gets the analog value for the analog stick 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - N/A
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void analogStick::getX()
{
    this->xV = analogRead(this->pins.x);
}


/*---------------------------------------------------------------------------------------------------------------------
| getX: Gets the analog value for the analog stick 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - N/A
|        -------------------------------------------------------------------------------------------------------
| Returns: nothing
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void analogStick::getY()
{
    this->yV = analogRead(this->pins.y);
}


/*---------------------------------------------------------------------------------------------------------------------
| getDirX: Uses the threshold to determine if the analog stick is considered going in a certain direction.
| This is then recorded into the object variables 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - N/A
|        -------------------------------------------------------------------------------------------------------
| Returns: One of 3 ASCII characters; 'l' for left, 'r' for right, or 'n' for neutral
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
char analogStick::getDirX()
{
    this->getX();

    if(this->xV > (ADC_RES/2) + this->threshold)
    {
    this->status.x = 'r';
    }
    else if(this->xV < (ADC_RES/2) - this->threshold)
    {
    this->status.x = 'l';
    }
    else
    {
    this->status.x = 'n';
    }

    return this->status.x;
}

/*---------------------------------------------------------------------------------------------------------------------
| getDirX: Uses the threshold to determine if the analog stick is considered going in a certain direction.
| This is then recorded into the object variables 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - N/A
|        -------------------------------------------------------------------------------------------------------
| Returns: One of 3 ASCII characters; 'u' for up, 'd' for down, or 'n' for neutral
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
char analogStick::getDirY()
{
    this->getY();

    if(this->yV > (ADC_RES/2) + this->threshold)
    {
    this->status.y = 'd';
    }
    else if(this->yV < (ADC_RES/2) - this->threshold)
    {
    this->status.y = 'u';
    }
    else
    {
    this->status.y = 'n';
    }

    return this->status.y;
}


/*---------------------------------------------------------------------------------------------------------------------
| getButton: Checks the status of the analog button pin and returns its current status (on/off) 
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - N/A
|        -------------------------------------------------------------------------------------------------------
| Returns: 1 for on, 0 for off
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
char analogStick::getButton()
{
    this->status.button = 1^digitalRead(this->pins.button);
    return this->status.button;
}


/*---------------------------------------------------------------------------------------------------------------------
| getStatus: Updates all values for the analog stick and returns them as an analogInfo struct
|        -------------------------------------------------------------------------------------------------------
| Arguments:
| - N/A
|        -------------------------------------------------------------------------------------------------------
| Returns: analogInfo object containing direction of x and y, as well as button status
|
|        -------------------------------------------------------------------------------------------------------
| Notes/Future Changes:
| - 
|        -------------------------------------------------------------------------------------------------------
| Created by: Cameron Jupp
| Date:       December 29, 2022
| Edited:     {Date}  
----------------------------------------------------------------------------------------------------------------------*/
void analogStick::update()
{   
    // Keep the old status to compare
    this->lastStatus = this->status;

    this->getDirX();
    this->getDirY();
    this->getButton();

    // If a reading is the same and not neutral, mark it as held
    if(this->status.y == 'n')
    {
        this->held.y = 0;
    }
    else if(this->status.y == this->lastStatus.y)
    {
        this->held.y = 1;
    }

    if(this->status.x == 'n')
    {
        this->held.x = 0;
    }
    else if(this->status.x == this->lastStatus.x)
    {
        this->held.x = 1;
    }


    if(!this->status.button)
    {
        this->held.button = 0;
    }
    else if(this->status.button == this->lastStatus.button)
    {
        this->held.button = 1;
    }

}


// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //
/*
void executeX(analogStick * stick, char continuous, void (*functL) (void), void (*functR) (void))
{
    if(!stick->held.x || continuous)
    {
     switch (stick->status.x)
     {
      case 'l':
        functL();
      break;
  
      case 'r':
        functR();
      break;
     }
   }
}

void executeY(analogStick * stick, char continuous, void (*functU) (void), void (*functD) (void))
{
    if(!stick->held.y || continuous)
    {
        switch (stick->status.y)
        {
        case 'u':
            functU();
        break;

        case 'd':
            functD();
        break;
        }
    }
}
void executeB(analogStick * stick, void (*funct) (void))
{
    if(!stick->held.button && stick->status.button)
    {
        funct();
    }
}
*/

