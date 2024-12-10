#ifndef chordtype_h
#define chordtype_h

#include "interfaces/encoderEditable.h"
#include "chord/chord.h"

class ChordType : public EncoderEditable {
private:
  Chord* chord;
public:
  ChordType(Chord* chord_) {
    chord = chord_;
    num = 1;
  }
  void increment() {
    int chordType = (int)chord->getChordType();
    chordType = (chordType < 8) ? chordType + 1 : 0;
    chord->setChordType((chordTypes)chordType);
  }
  void decrement() {
    int chordType = (int)chord->getChordType();
    chordType = (chordType > 0) ? chordType - 1 : 8;
    chord->setChordType((chordTypes)chordType);
  }
  void setChord(Chord* chord_) {
    chord = chord_;
  }
};

#endif