/*--------------------------------------------------------------------------------------------------------------------
| {FILE NAME}: 
|
| Created by:     Cameron Jupp
| Date Started:   {Date}
--------------------------------------------------------------------------------------------------------------------*/
#include "Heating.h" 
#include <Arduino.h>

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------- / ___/  / /     / __  /  / _ /    / _\      / /     / ____/ ----------------------------- //
// ------------------------- / /_//  / /__   / /_/ /  / _  \   / /_\\    / /__    \__ \  ------------------------------ //
// ------------------------ /____/  /____/  /_____/  /_____/  /_/   \\  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------- / ___/  / /     /  \      / ____/  / ____/  / ___/  / ____/ ---------------------------- //
// -------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  ----------------------------- //
// ------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ------------------------------ //
// -------------------------------------------------------------------------------------------------------------------- //
    heater::heater(char pinNum)
    {
        this->pin = pinNum;
        pinMode(this->pin, OUTPUT);
    }

    char heater::toggle(char onOff)
    {
        digitalWrite(this->pin, onOff);
        this->status = onOff;
    }

    char heater::getStatus()
    {
        return this->status;
    }

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //