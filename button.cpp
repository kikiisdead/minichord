#include "button.h"
#include <Arduino.h>

Button::Button(int buttonPin_) {
  this->buttonPin = buttonPin_;
  buttonState = HIGH;
  lastButtonState = HIGH;
  pinMode(buttonPin, INPUT_PULLUP);
  timeSinceEvent = 0;
  output = 0;
}

void Button::update() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH && timeSinceEvent > 5) {  // button press inverted HIGH adn LOW because input pullup
    output = 1;
    timeSinceEvent = 0;
  } else if (buttonState == LOW && lastButtonState == LOW) {  // button sustain
    output = 2;
  } else if (buttonState == HIGH && lastButtonState == LOW && timeSinceEvent > 5) {  // button release
    output = 3;
    timeSinceEvent = 0;
  } else {
    output = 0;
  }
  lastButtonState = buttonState;
}

int Button::buttonCheck() {
  return output;
}