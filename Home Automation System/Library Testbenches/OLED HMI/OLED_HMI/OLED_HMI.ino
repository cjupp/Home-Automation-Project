

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Serial_Tools.h>


#define MAX_INPUT_CHARS   10



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

//Wire.begin(24, 21);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


char weekdays[7][3] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};




class yesNoInput
{
  public:

  #define YES_NO    0
  #define OK_CANCEL 1
  
  char index = 0;
  char message[30];
  char mode = YES_NO;

  void build()
  {
    display.clearDisplay();
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,0);     // Start at top-left corn

    display.write(this->message);
    
    
    
    display.setCursor(0,55);     // Start at top-left corn

    if(!this->index)
    {
      display.setTextColor(BLACK, WHITE); // Draw white text
    }
    
    if(!this->mode)
    {
      display.write(" YES ");
    }
    else
    {
      display.write(" OK ");
    }

    display.setTextColor(WHITE); // Draw white text
    display.write("          ");

    if(this->index)
    {
      display.setTextColor(BLACK, WHITE); // Draw white text
    }
    
    if(!this->mode)
    {
      display.write(" NO ");
    }
    else
    {
      display.write(" CANCEL ");
    }
    display.display();
  }

  void setMsg(char * msg)
  {
    strcpy(this->message, msg);
  }

  char setMode(char mode)
  {
    //if statement in future to limit possibilities
    this->mode = mode;
  }
  

  char sel()
  {
    return this->index;
  }

  void left()
  {
    if(this->index)
    {
      this->index = 0;
    }
  }

  void right()
  {
    if(!this->index)
    {
      this->index = 1;      
    }

  }

};


class alphaInput
{
  public:
  #define X 0
  #define Y 1

  #define X_CHARS 11
  #define Y_CHARS 4

  char alphabet[Y_CHARS][X_CHARS+1] = {"0123456789","abcdefghijk","lmnopqrstuv","wxyz_<>,./?"};
  
  char output[MAX_INPUT_CHARS+1];

  char index[2] = {0, 0};

  char charIndex = 0;

  char * getStr()
  {
    return output;
  }

  void build()
  {
    display.clearDisplay();
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,0);     // Start at top-left corn
  
    this->dispOutput();
    
    this->dispChars();
  
    display.display();
  }

  void move(char dir)
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
  
  void sel()
  {
    if(charIndex < MAX_INPUT_CHARS-1)
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

  void del()
  {
    
    if(charIndex)
    {
      charIndex--;
    }
    output[charIndex] = 0;
  }

  void dispOutput()
  {
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE);
    for(int i = 0; i < charIndex; i++)
    {
      display.write(output[i]);
    }
    if (charIndex < MAX_INPUT_CHARS-1)
    {
      display.setTextColor(BLACK, WHITE);
      display.write(" ");
    }
    else if(charIndex == MAX_INPUT_CHARS-1)
    {
      
    }
    display.write("\n\n");
  }

  void dispChars()
  {
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE);
    
    for(int y = 0; y < Y_CHARS; y++)
    {
      for(int x = 0; x < X_CHARS; x++)
      {
        if(x == index[X] && y == index[Y])
        {
          display.setTextColor(BLACK, WHITE);
        }
        display.write(alphabet[y][x]);

        display.setTextColor(WHITE);
        if(x < X_CHARS-1)
        {
          display.write(" ");
        }
        
      }
      display.write("\n");
    }
  }
  
};


//------------------------------------------------------------------------
class timeInput
{
  public:

  char index = 0;

  char output[4];

  char hr = 0;
  char min = 0;

  char time[2];

  void build()
  {
    display.clearDisplay();
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,20);     // Start at top-left corn
    display.setTextSize(3);      // Normal 1:1 pixel scale

    display.printf(" %i%i:%i%i", this->output[0], this->output[1], this->output[2], this->output[3]);
    
    display.write("\n");

    display.write(" ");
    for(int i = 0; i < this->index; i++)
    {
        display.write(" ");
        if(i==1)display.write(" ");
        
    }

      display.write("^");

    
    display.display();
  }

  void up()
  {
    if(this->index%2)
    {
        if(this->output[index] < 9)
        {
          this->output[index]++;
        }
        else
        {
          this->output[index] = 0;
        }
    }
    else if(this->index == 0)
    {
        if(this->output[index] < 2)
        {
          this->output[index]++;
        }
        else
        {
          this->output[index] = 0;
        }
    }
    else if(this->index == 2)
    {
        if(this->output[index] < 5)
        {
          this->output[index]++;
        }
        else
        {
          this->output[index] = 0;
        }      
    }

  }

  void down()
  {
    if(this->index%2)
    {
        if(this->output[index] > 0)
        {
          this->output[index]--;
        }
        else
        {
          this->output[index] = 9;
        }
    }
    else if(this->index == 0)
    {
        if(this->output[index] > 0)
        {
          this->output[index]--;
        }
        else
        {
          this->output[index] = 2;
        }
    }
    else if(this->index == 2)
    {
        if(this->output[index] > 0)
        {
          this->output[index]--;
        }
        else
        {
          this->output[index] = 5;
        }      
    }
  }

  void left()
  {
    if(this->index)
    {
      this->index--;
    }
    else
    {
      this->index = 3;
    }
  }

  void right()
  {
        if(this->index < 3)
    {
      this->index++;
    }
    else
    {
      this->index = 0;
    }
  }

  void sel()
  {
    
  }

};

class weekdayInput
{
  public:
  char days = 0;
  char index = 0;

  void build()
  {
    display.clearDisplay();
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,20);     // Start in middle
    display.setTextSize(1);      // Normal 1:1 pixel scale

    // Display weekdays with chosen ones highlighted
    for(int i = 0; i < 7; i++)
    {
      if((BIT0<<i) & this->days)
      {    
        display.setTextColor(BLACK, WHITE); 
      }
      else
      {
        display.setTextColor(WHITE);
      }
      display.write(weekdays[i]);
      display.setTextColor(WHITE);
      display.write(" ");
    }

    // Display selection arrow underneath
    display.setTextColor(WHITE);
    display.write("\n\n");
    for(int i = 0; i < this->index; i++)
    {
      display.write("   ");
        
    }

    display.write(" ^");
    display.display();
    
  }

  void left()
  {
    if(this->index > 0)
    {
      this->index--;
    }
    else
    {
      this->index = 6;
    }
  }

  void right()
  {
    if(this->index < 6)
    {
      this->index++;
    }
    else
    {
      this->index = 0;
    }
  }

  void toggle()
  {
    this->days ^= (BIT0<<this->index);
  }

  char start()
  {
    
  }
  
};

class listMenu

alphaInput testAlpha;
timeInput testTime;
weekdayInput testWeek;
yesNoInput testYN;

void alphaMenu()
{
  testAlpha.build();
  if(Serial.available())
  {
    switch(Serial.read())
    {
      case 'u':
        testAlpha.move('u');
      break;
        
      case 'd':
        testAlpha.move('d');
      break;
  
      case 'l':
        testAlpha.move('l');
      break;
  
      case 'r':
        testAlpha.move('r');
      break;

      case 'c':
        testAlpha.sel();
      break;

      case 'b':
        testAlpha.del();
      break;
  
      default:
        
      break;
    }
  }
}

void clkMenu()
{
      testTime.build();
    if(Serial.available())
  {
    switch(Serial.read())
    {
      case 'u':
        testTime.up();
      break;
        
      case 'd':
        testTime.down();
      break;
  
      case 'l':
        testTime.left();
      break;
  
      case 'r':
        testTime.right();
      break;

      case 'c':
        testTime.sel();
      break;
  
      default:
        
      break;
    }
  }
}

void weekMenu()
{
  testWeek.build();
  if(Serial.available())
  {
    switch(Serial.read())
    {
      case 'l':
        testWeek.left();
      break;
  
      case 'r':
        testWeek.right();
      break;

      case 'c':
        testWeek.toggle();
      break;
  
      default:
        
      break;
    }
  }
}

void ynMenu()
{
  testYN.build();
  if(Serial.available())
  {
    switch(Serial.read())
    { 
      case 'l':
        testYN.left();
      break;
  
      case 'r':
        testYN.right();
      break;

      case 'c':
        testYN.sel();
      break;
  
      default:
        
      break;
    }
  }
}

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

  display.display();

  testYN.setMsg("Would you like to connect to WiFi?");

}

void loop() 
{
  ynMenu();


}
