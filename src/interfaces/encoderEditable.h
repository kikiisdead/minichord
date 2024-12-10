#ifndef encoderEditable_h
#define encoderEditable_h

class EncoderEditable {
public:
  int num;
  virtual void increment() = 0;
  virtual void decrement() = 0;
  int getNum() {
    return num;
  }
};

#endif