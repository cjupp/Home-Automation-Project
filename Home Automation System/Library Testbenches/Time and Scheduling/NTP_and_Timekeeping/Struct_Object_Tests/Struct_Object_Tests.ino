#include <stdio.h>
#include <string.h>

#define MAX_NAME_CHAR 10
#define MAX_EVENTS    10

struct clkTime
{
  char hour;

  char min;
};

class event
{
  public:

  char configd = 0;

  char name[MAX_NAME_CHAR];

  clkTime startTime;

  clkTime duration;

  char weekDays = 0;

  char active = 0;

  void (*action)(void);

  void config(char * namePtr, clkTime time, clkTime duration, void (*functPtr) (void))
  {
    strcpy(this->name, namePtr);

    this->startTime = time;

    this->duration = duration;

    this->action = functPtr;
  }

  void del()
  {
    
  }

  void activate()
  {
    
  }

  void deactivate()
  {
    
  }

};


event testEvents[5];

void orderEvents(char delIndex )
{
    testEvents[delIndex] = testEvents[delIndex+1];
}

void dispEvent(event * eventPtr)
{
  Serial.print("Name: ");
  Serial.println(eventPtr->name);

  Serial.print("Time: ");
  Serial.println(eventPtr->startTime.hour, DEC);
  Serial.println(eventPtr->startTime.min, DEC);
  
}


void setup() {
  Serial.begin(115200);
    
  testEvents[0].config("object 0", {1, 30}, {0, 30}, NULL);
  dispEvent(&testEvents[0]);

  testEvents[1].config("object 1", {2, 45}, {3, 6}, NULL);
  dispEvent(&testEvents[1]);

  testEvents[0] = testEvents[1];
  dispEvent(&testEvents[0]);

}

void loop() {
  // put your main code here, to run repeatedly:

}
