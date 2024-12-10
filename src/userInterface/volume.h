#ifndef volume_h
#define volume_h

#include "interfaces/encoderEditable.h"

class Volume : public EncoderEditable {
private:
  float volume;
public:
  Volume(float volume_) {
    volume = volume_;
    num = 2;
  }
  void increment() {
    volume = (volume < 1) ? volume + 0.01 : 1;
  }
  void decrement() {
    volume = (volume > 0) ? volume - 0.01 : 0;
  }
  float getVolume() {
    return volume;
  }
};

#endif