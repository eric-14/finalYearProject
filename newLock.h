#ifndef NEWLOCK_H
#define NEWLOCK_H
#include <Arduino.h>
class Lock{
  
  public:
    int lock(int open);
    Lock(int pin){
        lockpin=pin;
        pinMode(lockpin,OUTPUT);
  }
  private:
    int lockpin;
};

#endif


