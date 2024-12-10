#ifndef betterEncoder_h
#define betterEncoder_h

#include <Encoder.h>

class BetterEncoder {
private:
  Encoder encoder;
  int oldPosition;
  int sens;
public:
  BetterEncoder(int pin1, int pin2, int sens_);
  void incrementHandler(void (*func)());
  void decrementHandler(void (*func)());
  void (*increment)();
  void (*decrement)();
  void process();
};

#endif