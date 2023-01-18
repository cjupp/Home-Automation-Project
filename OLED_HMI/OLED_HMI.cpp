/*--------------------------------------------------------------------------------------------------------------------
| oled_hmi.cpp: 
|
| Created by:     Cameron Jupp
| Date Started:   December 30, 2022
--------------------------------------------------------------------------------------------------------------------*/ 

#include "OLED_HMI.h"
#include "OLED_Tools.h"
#include "Real_Timekeeping.h"
#include "stdio.h"
#include <Arduino.h>

// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------- / ___/  / /     / __  /  / _ /    / _\      / /     / ____/ ----------------------------- //
// ------------------------- / /_//  / /__   / /_/ /  / _  \   / /_\\    / /__    \__ \  ------------------------------ //
// ------------------------ /____/  /____/  /_____/  /_____/  /_/   \\  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// -------------------------------------------------------------------------------------------------------------------- //
// --------------------------- /    \   / ___/  / ___/  /_  _/  / \  / /  / ___/  / ____/ ----------------------------- //
// -------------------------- / /_\ /  / __/   / __/     / /   / /\\/ /  / __/    \__ \  ------------------------------ //
// ------------------------- /_____/  /____/  /_/      /___/  /_/  \_/  /____/  /_____/ ------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// -------------------------------------------------------------------------------------------------------------------- //
// ----------------------------- / ___/  / /     / _\      / ____/  / ____/  / ___/  / ____/ -------------------------- //
// ---------------------------- / /__   / /__   / /_\\     \__ \    \__ \   / __/    \__ \  --------------------------- //
// --------------------------- /____/  /____/  /_/   \\  /_____/  /_____/  /____/  /_____/ ---------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------ ALPHAINPUT ---------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //
  char * strInput::getStr()
  {
    return output;
  }

  void strInput::changeCase(char charCase)
  {
    for(int y = 0; y < Y_CHARS; y++)
    {
      for(int x = 0; x < X_CHARS; x++)
      {

        if(charCase && this->alphabet[y][x] >= 65 && this->alphabet[y][x] <= 90)
        {
          this->alphabet[y][x]+= 32;
        }

        if(!charCase && this->alphabet[y][x] >= 97 && this->alphabet[y][x] <= 122)
        {
          this->alphabet[y][x]-= 32;
        }

      }
    }
  }

  

  void strInput::build()
  {
    oledDisplay.clearDisplay();
    oledDisplay.setTextColor(WHITE); // Draw white text
    oledDisplay.setCursor(0,0);     // Start at top-left corn
  
    this->dispOutput();
    
    this->dispChars();
  
    oledDisplay.display();
  }

  void strInput::up()
  {

  }

  void strInput::down()
  {
    
  }

  void strInput::left()
  {
    
  }

  void strInput::right()
  {
    
  }
  
  void strInput::move(char dir)
  {
    switch(dir)
    {
      case 'd':

        if(this->index[Y] == Y_CHARS-1)
        {
          this->index[Y] = 0;        
        }
        else if(this->index[Y] < Y_CHARS-1)
        {
          this->index[Y]++;
        }

      break;

      case 'u':
        if(!this->index[Y])
        {
          this->index[Y] = Y_CHARS-1;        
        }
        else if(this->index[Y] > 0)
        {
          this->index[Y]--;
        }
      break;

      case 'l':
      if(!this->index[X])
        {
          this->index[X] = X_CHARS-1;        
        }
        else if(this->index[X] > 0)
        {
          this->index[X]--;
        }
      break;

      case 'r':
      if(this->index[X] == X_CHARS-1)
        {
          this->index[X] = 0;        
        }
        else if(this->index[X] < X_CHARS-1)
        {
          this->index[X]++;
        }
      break;
    }
  }
  
  void strInput::sel()
  {
    // Accept and Go Back conditions
    // If in the last row
    if(this->index[Y] == Y_CHARS-1)
    {
      // If in the first column
      if(!this->index[X])
      {
        this->cancel();
      }
      // Or if in the last column
      else if(this->index[X] == X_CHARS-1)
      {
        this->accept();
      }

      // Caps condition
      else if(this->index[X] == 5)
      {
        if(this->caps)
        {
          this->caps = 0;
        }
        else
        {
          this->caps = 1;
        }
        this->changeCase(this->caps);
        Serial.println(this->caps, DEC);
      }
    }


    // Backspace condition
    else if(!this->index[Y] && this->index[X] == X_CHARS-1)
    {
      this->del();
    }
    
    else
    {
        if(charIndex < OLED_INPUT_CHARS-1)
        {
        if(alphabet[this->index[Y]][this->index[X]])
        {
            output[charIndex] = alphabet[this->index[Y]][this->index[X]];
        }
        else
        {
            output[charIndex] = ' ';
        }
        charIndex++;
        }
    }

  }

  void strInput::del()
  {
    
    if(charIndex)
    {
      charIndex--;
    }
    output[charIndex] = 0;
  }

  void strInput::accept()
  {
    this->complete = 1;
  }

  void strInput::cancel()
  {
    //this->reset();
    this->complete = -1;
  }

  void strInput::reset()
  {
    this->index[X] = 0;
    this->index[Y] = 0;
    this->charIndex = 0;

    this->complete = 0;
    
    for(int i = 0; i < OLED_INPUT_CHARS+1; i++)
    {
      this->output[i] = 0;
    }
  }

  void strInput::dispOutput()
  {
    oledDisplay.setTextSize(1);      // Normal 1:1 pixel scale
    oledDisplay.setTextColor(WHITE);
    for(int i = 0; i < charIndex; i++)
    {
      oledDisplay.write(output[i]);
    }
    if (charIndex < OLED_INPUT_CHARS-1)
    {
      oledDisplay.setTextColor(BLACK, WHITE);
      oledDisplay.write(" ");
    }
    else if(charIndex == OLED_INPUT_CHARS-1)
    {
      
    }
    oledDisplay.write("\n\n");
  }

  void strInput::dispChars()
  {
    oledDisplay.setTextSize(1);      // Normal 1:1 pixel scale
    oledDisplay.setTextColor(WHITE);
    
    for(int y = 0; y < Y_CHARS; y++)
    {
      for(int x = 0; x < X_CHARS; x++)
      {
        if(x == index[X] && y == index[Y])
        {
          oledDisplay.setTextColor(BLACK, WHITE);
        }
        oledDisplay.write(alphabet[y][x]);

        oledDisplay.setTextColor(WHITE);
        if(x < X_CHARS-1)
        {
          oledDisplay.write(" ");
        }
        
      }
      oledDisplay.write("\n");
    }
  }

// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------- TIMEINPUT ----------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //
char timeInput::setHeader(char * newHeader)
{
  strcpy(this->header, newHeader);
}

void timeInput::build()
{
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE); // Draw white text
  oledDisplay.setCursor(30,0);     // Start at top-left corn
  oledDisplay.setTextSize(1);      // Normal 1:1 pixel scale

  // Display the header if there is one
  oledDisplay.write(this->header);

  oledDisplay.setCursor(0,20);     // Start at top-left corn
  oledDisplay.setTextSize(3);      // Normal 1:1 pixel scale

  //oledDisplay.printf(" %i%i:%i%i", this->output[0], this->output[1], this->output[2], this->output[3]);

  oledDisplay.write(" ");
  for(int i = 0; i < 4; i++)
  {
    if(this->index == i+1)
    {
      oledDisplay.setTextColor(BLACK, WHITE);
    }
    else
    {
      oledDisplay.setTextColor(WHITE);
    }
    oledDisplay.printf("%i", this->output[i]);
    if(i==1)
    {
      oledDisplay.setTextColor(WHITE);
      oledDisplay.write(":");
    }
    
  }
  //-------------------------------------------------------------------
  oledDisplay.write("\n");
  oledDisplay.setTextSize(2);      // Normal 1:1 pixel scale 
  if(!this->index)
  {
    oledDisplay.setTextColor(BLACK, WHITE);
  }
  else
  {
    oledDisplay.setTextColor(WHITE);
  }
  oledDisplay.write("<");

  oledDisplay.setTextColor(WHITE);
  oledDisplay.write("        ");
  
  if(this->index == 5)
  {
    oledDisplay.setTextColor(BLACK, WHITE);
  }
  else
  {
    oledDisplay.setTextColor(WHITE);
  }
  oledDisplay.write(">"); 

  
  oledDisplay.display();
}

void timeInput::up()
{
  char lim = 9;
  if(this->output[1] == 2)
  {
    lim = 3; 
  }

  switch (this->index)
  {
    case 1:
      if(this->output[0] < 2)
      {
        this->output[0]++;
      }
      else
      {
        this->output[0] = 0;
      }
    break;

    case 2:
      if(this->output[1] < lim)
      {
        this->output[1]++;
      }
      else
      {
        this->output[1] = 0;
      }
    break;

    case 3:
      if(this->output[2] < 5)
      {
        this->output[2]++;
      }
      else
      {
        this->output[2] = 0;
      }
    break;

    case 4:
      if(this->output[3] < 9)
      {
        this->output[3]++;
      }
      else
      {
        this->output[3] = 0;
      }
    break;
  }
  
  if(this->output[0] == 2 && this->output[1] > 3)
  {
    this->output[1] = 3;
  }
}

void timeInput::down()
{
  char lim = 9;
  if(this->output[1] == 2)
  {
    lim = 3; 
  }
  
  switch(this->index)
  {
    case 1:
      if(this->output[0] > 0)
      {
        this->output[0]--;
      }
      else
      {
        this->output[0] = 2;
        
      }
    break;

    case 2:
      if(this->output[1] > 0)
      {
        this->output[1]--;
      }
      else
      {
        this->output[1] = lim;
      }
    break;

    case 3:
      if(this->output[2] > 0)
      {
        this->output[2]--;
      }
      else
      {
        this->output[2] = 5;
      } 
    break;

    case 4:
      if(this->output[3] > 0)
      {
        this->output[3]--;
      }
      else
      {
        this->output[3] = 9;
      }
    break;
  }
  
  if(this->output[0] == 2 && this->output[1] > 3)
  {
    this->output[1] = 3;
  }
}

void timeInput::left()
{
  if(this->index)
  {
    this->index--;
  }
}

void timeInput::right()
{
  if(this->index < 5)
  {
    this->index++;
  }
}

void timeInput::sel()
{
  if(!this->index)
  {
    this->cancel();
  }

  else if(this->index == 5)
  {
    this->accept();
  }
  
  else
  {
    this->index++;
  }
}

void timeInput::convertTime()
{
  this->clock.hr = (10*(this->output[0]))+(this->output[1]);
  this->clock.min = (10*(this->output[2]))+(this->output[3]);
}

clkTime * timeInput::accept()
{
  this->complete = 1;

  this->convertTime();

  return &this->clock;
}

void timeInput::cancel()
{
  this->complete = -1;
}

void timeInput::reset()
{
  this->complete = 0;
  this->index = 0;

  for(int i = 0; i < 4; i++)
  {
    this->output[i] = 0;
  }
  
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------- WEEKDAYINPUT ---------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //

void weekdayInput::build()
{
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE); // Draw white text
  oledDisplay.setCursor(0,20);     // Start in middle
  oledDisplay.setTextSize(1);      // Normal 1:1 pixel scale

  // Display weekdays with chosen ones highlighted
  for(int i = 0; i < 7; i++)
  {
    if((BIT0<<i) & this->days)
    {    
      oledDisplay.setTextColor(BLACK, WHITE); 
    }
    else
    {
      oledDisplay.setTextColor(WHITE);
    }
    oledDisplay.write(hmiWeekdays[i]);
    oledDisplay.setTextColor(WHITE);
    oledDisplay.write(" ");
  }
  
  //-------------------------------------------------------------------
  
  // Display selection arrow underneath
  oledDisplay.setTextColor(WHITE);
  oledDisplay.write("\n\n");

  if(this->index && this->index < 8)
  {
    
    for(int i = 0; i < this->index-1; i++)
    {
      oledDisplay.write("   ");  
    }
    oledDisplay.write(" ^");
  }
  else
  {
    oledDisplay.write("                ");
  }

  

  //-------------------------------------------------------------------
  oledDisplay.write("\n");
    
  if(!this->index)
  {
    oledDisplay.setTextColor(BLACK, WHITE);
  }
  else
  {
    oledDisplay.setTextColor(WHITE);
  }
  oledDisplay.write("<");

  oledDisplay.setTextColor(WHITE);
  oledDisplay.write("                  ");
  
  if(this->index == 8)
  {
    oledDisplay.setTextColor(BLACK, WHITE);
  }
  else
  {
    oledDisplay.setTextColor(WHITE);
  }
  oledDisplay.write(">"); 
  
  oledDisplay.display();
}


void weekdayInput::left()
{
  if(this->index > 0)
  {
    this->index--;
  }
}

void weekdayInput::right()
{
  if(this->index < 8)
  {
    this->index++;
  }

}

void weekdayInput::sel()
{
  if(this->index && this->index < 8)
  {
    this->days ^= (BIT0<<this->index-1); 
  }
  else if(!this->index)
  {
    this->cancel();
  }
  else if(this->index == 8)
  {
    this->accept();
  }
}

void weekdayInput::cancel()
{
  this->complete = -1;
}

char weekdayInput::accept()
{
  this->complete = 1;
  return this->days;
}

void weekdayInput::reset()
{
  this->complete = 0;
  this->index = 0;
  this->days = 0;
}



// ---------------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------------ YESNOINPUT ---------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------------- //
void yesNoInput::build()
{
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE); // Draw white text
  oledDisplay.setCursor(0,0);     // Start at top-left corn

  oledDisplay.write(this->message);
  
  oledDisplay.setCursor(0,55);     // Start at top-left corn

  if(!this->index)
  {
    oledDisplay.setTextColor(BLACK, WHITE); // Draw white text
  }
  
  if(!this->mode)
  {
    oledDisplay.write(" YES ");
  }
  else
  {
    oledDisplay.write(" OK ");
  }

  oledDisplay.setTextColor(WHITE); // Draw white text
  oledDisplay.write("          ");

  if(this->index)
  {
    oledDisplay.setTextColor(BLACK, WHITE); // Draw white text
  }
  
  if(!this->mode)
  {
    oledDisplay.write(" NO ");
  }
  else
  {
    oledDisplay.write(" CANCEL ");
  }
  oledDisplay.display();
}

void yesNoInput::setMsg(char * msg)
{
  strcpy(this->message, msg);
}

char yesNoInput::setMode(char mode)
{
  //if statement in future to limit possibilities
  this->mode = mode;
}


char yesNoInput::sel()
{
  this->complete = this->index+1;
  return this->complete;
}

void yesNoInput::left()
{
  if(this->index)
  {
    this->index = 0;
  }
}

void yesNoInput::right()
{
  if(!this->index)
  {
    this->index = 1;      
  }

}


// -------------------------------------------------------------------------------------------------------------------- //
// -------------------  / ___/  / / //  / \  / /  / ___/ /__  __/  /_  _/  / __  /  / \  / /  / ____/ ----------------- //
// ------------------  / __/   / /_//  / /\\/ /  / /__     / /      / /   / /_/ /  / /\\/ /   \__ \ ------------------- //
// -----------------  /_/     /____/  /_/  \_/  /____/    /_/     /___/  /_____/  /_/  \_/  /_____/ ------------------- //
// -------------------------------------------------------------------------------------------------------------------- //
void hmiSystemInit()
{
// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
if(!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
{ 
  Serial.println(F("SSD1306 allocation failed"));
  for(;;); // Don't proceed, loop forever
}

  oledDisplay.cp437(true);         // Use full 256 char 'Code Page 437' font

  oledDisplay.clearDisplay();
}