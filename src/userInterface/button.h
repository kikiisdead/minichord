#ifndef button_h
#define button_h

#include <Arduino.h>

class Button {
private:
  int buttonPin;
  int buttonState;
  int lastButtonState;
  int output;
  elapsedMillis timeSinceEvent; // used as a debounce to avoid using delays
public:
  Button(int buttonPin_);
  void update();
  int buttonCheck();  //returns an integer based on the result
};

#endif