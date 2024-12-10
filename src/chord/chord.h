#ifndef chord_h
#define chord_h

#include <Arduino.h>
#include <map>
#include <vector>
#include <algorithm>
#include "userInterface/button.h"

#define SUSTAIN true
#define STRUM false

enum chordTypes { MAJOR, MINOR, AUGMENTED, DIMINISHED, MAJORSEVEN, MINORSEVEN, DOMINANTSEVEN, HALFDIMINISHEDSEVEN, FULLDIMINISHEDSEVEN };

class Chord {
private:
  std::map<chordTypes, std::vector<int>> noteMap;
  std::vector<int> possibleNotes;
  int sustainPitches[3];  //pitches for the sustained chord
  std::vector<int> strumPitches;    //pitches for strumming
  int root;
  int noteTouch[8];
  int prevNoteTouch[8];
  Button chordButton;
  chordTypes type;
  void chordPress();
  void chordRelease();
  void chordSustain();

public:
  Chord(int chordPin);
  void setChordType(chordTypes type);
  chordTypes getChordType();
  void initRoot(int root_);
  void setRoot(int root);
  int getRoot();
  void populateVector();
  void update();
  void (*noteOn)(int, int, bool);
  void (*noteOff)(int, int, bool);
  void (*capCheck)(int*); //will return the position of the touch from the trill back into the program
  void noteOnHandler(void (*func)(int, int, bool));
  void noteOffHandler(void (*func)(int, int, bool));
  void capCheckHandler(void (*func)(int*));
  Button* getButton();
};

#endif