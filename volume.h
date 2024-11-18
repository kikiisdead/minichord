#ifndef volume_h
#define volume_h

#include "encoderEditable.h"

class Volume : public EncoderEditable {
private:
  float volume;
public:
  Volume(float volume_) {
    volume = volume_;
    num = 2;
  }
  void increment() {
    if (volume < 1) {
      volume += 0.01;
    }
  }
  void decrement() {
    if (volume > 0) {
      volume -= 0.01;
    }
  }
  float getVolume() {
    return volume;
  }
};

#endif