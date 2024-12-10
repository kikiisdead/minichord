#ifndef chordroot_h
#define chordroot_h

#include "interfaces/encoderEditable.h"
#include "chord/chord.h"

class ChordRoot : public EncoderEditable {
private:
  Chord* chord;
public:
  ChordRoot(Chord* chord_) {
    chord = chord_;
    num = 0;
  }
  void increment() {
    chord->setRoot(chord->getRoot() + 1);
  }
  void decrement() {
    chord->setRoot(chord->getRoot() - 1);
  }
  void setChord(Chord* chord_) {
    chord = chord_;
  }
};

#endif