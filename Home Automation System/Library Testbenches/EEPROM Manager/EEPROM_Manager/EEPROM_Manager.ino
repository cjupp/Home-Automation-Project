// FLASH tools

//Sources say the memory can be up to 1048 bytes, but 512 is used here to be safe
#define MAX_EEPROM 512

int loadIndex = 0;

// List to keep track of data structure pointers
char * ptrList[10];

char writeEEPROM(char * dataPtr, int size)
{
  //check size + load index is less than total memory siz
  
  //for loop

  //load index = load index + size
}

eraseEEPROM()
{
  // erase from one ptr to the next

  // reorganize data  based on pointer array

  // update loadIndex
}

loadEEPROM()
{
  // probably just a single line of code passing the arguments into the eeprom library call
}








void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
