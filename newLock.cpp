#include "newLock.h"
int Lock::lock(int open){
  if(open == 1)
  {
    //Serial.println("\n Opening the door");
    digitalWrite(lockpin,HIGH);
  }
  else if(0 == open){
   // Serial.print("\n Door locked ");
    digitalWrite(lockpin, LOW);

  }

}