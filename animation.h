#ifndef animation_h
#define animation_h

#include "encoderEditable.h"

class Animation : public EncoderEditable {
public:
  Animation() {
    num = 4;
  }
  void increment() {
  }
  void decrement() {
  }
};

#endif