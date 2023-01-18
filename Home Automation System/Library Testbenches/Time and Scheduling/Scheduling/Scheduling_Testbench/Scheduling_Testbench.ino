#include <Arduino.h>


#define MAX_NAME_CHAR 10
#define MAX_EVENTS    10

#define SUN         BIT0
#define MON         BIT1
#define TUE         BIT2
#define WED         BIT3
#define THU         BIT4
#define FRI         BIT5
#define SAT         BIT6
#define EVERYDAY    BIT7


struct clkTime
{
  char hr;

  char min;
};


class event
{
  public:

  char configd = 0;

  char name[MAX_NAME_CHAR];

  clkTime startTime;

  clkTime endTime;

  char weekDays = 0;

  char active = 0;

  void (*action)(char);

  void config(char * namePtr, clkTime time, clkTime endTime, char weekdays, void (*functPtr) (char))
  {
    strcpy(this->name, namePtr);

    this->startTime = time;

    this->endTime = endTime;

    this->weekDays = weekdays;

    this->action = functPtr;

    this->configd = 1;
  }

  void del()
  {
    this->configd = 0;
  }

  void activate()
  {
    this->action(1);
  }

  void deactivate()
  {
    this->action(0);
  }

};


class eventHandler
{
  public:

  event events[MAX_EVENTS];

  char numConfigd = 0;


  char addEvent(char * name, clkTime time, clkTime endTime, char weekdays, void (*functPtr) (char))
  {
    // If all of the events have been configured already
    if(this->numConfigd == MAX_EVENTS)
    {
      return -1;
    }
    // Otherwise, configure the next one
    else
    {
      this->events[numConfigd++].config(name, time, endTime, weekdays, functPtr);
      return 0;
    }
  }

  // After an event is deleted, shift everything so that it is continuous and predictable
  void orderEvents(char delIndex)
  {
    // For every event after the deleted one and up to the last one configured
    for(int i = delIndex; i < numConfigd - 1; i++)
    {
      // The current event is overwritten by the next one
      this->events[i] = this->events[i+1];
    }
    // When the last configured event is reached, mark as unconfigd (delete)
    this->events[numConfigd-1].del();
  }

  // Remove an already configured event
  char deleteEvent(char delIndex)
  {
    // If there is an event to delete
    if(numConfigd)
    {
      // If the event to be deleted is in the middle of the array
      if(delIndex < numConfigd-1)
      {
        // Shift the whole array forward to fill in the gap
        this->orderEvents(delIndex);
        

        // Decriment the number configd
        this->numConfigd--;
        
      }
      // Else if the event to be deleted is the last one in the array
      else
      {
        // Set the configd bit to 0 and decriment the number configd
        this->events[delIndex].del();
        this->numConfigd--;
      }

    }
    else
    {
      // No events to delete, return error
      return -1;
    }

  }

  // Check for events activating
  void check(char currHour, char currMin, char currDay)
  {
    for (int i = 0; i < this->numConfigd; i++)
    {
      // If the passed in hour and minute time match, as well as the weekday, activate the event
      if(this->events[i].startTime.hr == currHour && this->events[i].startTime.min ==currMin && this->events[i].weekDays & currDay)
      {
        this->events[i].activate();
      }
      // If the start time of the event plus the duration equals the passed in time, deactivate the event
      // The day of the week does not matter, as the event could be active until the next day possibly
      if(this->events[i].endTime.hr == currHour && this->events[i].endTime.min ==currMin)
      {
        this->events[i].deactivate();
      }
    }
  }
  
};


void printEvent(event * eventPtr)
{
  Serial.println("\nName: ");
  Serial.println(eventPtr->name);

  Serial.println("Configd: ");
  Serial.println(eventPtr->configd, DEC);  

  Serial.println("Start Time: ");
  Serial.println(eventPtr->startTime.hr, DEC);
  Serial.println(eventPtr->startTime.min, DEC);

  Serial.println("End Time");
  Serial.println(eventPtr->endTime.hr, DEC);
  Serial.println(eventPtr->endTime.min, DEC);

  Serial.println("Weekdays");
  Serial.println(eventPtr->weekDays, BIN);

  Serial.println();
}





eventHandler testScheduler;


void setup() 
{
  Serial.begin(115200);
  
  testScheduler.addEvent("event 0", {1, 30}, {0, 30}, 0b00111110, NULL);
    
  testScheduler.addEvent("event 1", {2, 30}, {1, 30}, 0b00111110, NULL);
    
  testScheduler.addEvent("event 2", {3, 30}, {2, 30}, 0b00111110, NULL);
    
  testScheduler.addEvent("event 3", {4, 30}, {3, 30}, 0b00111110, NULL);
    
  testScheduler.addEvent("event 4", {5, 30}, {4, 30}, 0b00111110, NULL);
  
  printEvent(&testScheduler.events[0]);
  printEvent(&testScheduler.events[1]);
  printEvent(&testScheduler.events[2]);
  printEvent(&testScheduler.events[3]);
  printEvent(&testScheduler.events[4]);
  printEvent(&testScheduler.events[5]);
  printEvent(&testScheduler.events[6]);
  
  Serial.print("Number of configured events: ");
  Serial.println(testScheduler.numConfigd, DEC);
  
  Serial.println("\n Deleting event 2\n");
  testScheduler.deleteEvent(2);
  
  printEvent(&testScheduler.events[0]);
  printEvent(&testScheduler.events[1]);
  printEvent(&testScheduler.events[2]);
  printEvent(&testScheduler.events[3]);
  printEvent(&testScheduler.events[4]);
  printEvent(&testScheduler.events[5]);
  printEvent(&testScheduler.events[6]);

  Serial.print("Number of configured events: ");
  Serial.println(testScheduler.numConfigd, DEC);
  
  Serial.println("\n Deleting event 3\n");
  testScheduler.deleteEvent(3);
  
  printEvent(&testScheduler.events[0]);
  printEvent(&testScheduler.events[1]);
  printEvent(&testScheduler.events[2]);
  printEvent(&testScheduler.events[3]);
  printEvent(&testScheduler.events[4]);
  printEvent(&testScheduler.events[5]);
  printEvent(&testScheduler.events[6]);

  Serial.print("Number of configured events: ");
  Serial.println(testScheduler.numConfigd, DEC);

}

void loop() {
  // put your main code here, to run repeatedly:

}
