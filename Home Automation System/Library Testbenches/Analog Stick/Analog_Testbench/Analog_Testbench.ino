#include <Arduino.h>
#include <analog_stick.h>

#define BPIN  33
#define XPIN  34
#define YPIN  35



analogStick testStick(XPIN, YPIN, BPIN, 2000);

void dispAnalog()
{
  Serial.print("X: ");
  Serial.println(analogRead(XPIN), DEC);

  Serial.print("Y: ");
  Serial.println(
  analogRead(YPIN), DEC);

  Serial.print("Button: ");
  Serial.println(
  1^digitalRead(BPIN), DEC);

}

void dispDir()
{
  Serial.print("X direction: ");
  Serial.println(testStick.getDirX());

  Serial.print("Y direction: ");
  Serial.println(testStick.getDirY());

  Serial.println("--------------------------------");
}
  
void setup() 
{
  Serial.begin(115200);



}

void loop() {
  
  //dispDir();
  dispAnalog();



  delay(1000);

}
