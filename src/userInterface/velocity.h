#ifndef velocity_h
#define velocity_h

#include "interfaces/encoderEditable.h"

class Velocity : public EncoderEditable {
private:
  int velocity;
public:
  Velocity(int velocity_) {
    velocity = velocity_;
    num = 3;
  }
  void increment() {
    velocity = (velocity < 127) ? velocity + 1 : velocity;
  }
  void decrement() {
    velocity = (velocity > 0) ? velocity - 1 : velocity;
  }
  int getVelocity() {
    return velocity;
  }
};

#endif