#include "betterEncoder.h"

BetterEncoder::BetterEncoder(int pin1, int pin2, int sens_)
  : encoder(pin1, pin2) {
  oldPosition = 0;
  sens = sens_;
}

void BetterEncoder::incrementHandler(void (*func)()) {
  increment = *func;
}

void BetterEncoder::decrementHandler(void (*func)()) {
  decrement = *func;
}

void BetterEncoder::process() {
  int newPosition = encoder.read();

  if (newPosition - oldPosition >= sens) {
    oldPosition = newPosition;  // update the encoder's position
    decrement();
  }

  if (newPosition - oldPosition <= -sens) {
    oldPosition = newPosition;
    increment();
  }
}