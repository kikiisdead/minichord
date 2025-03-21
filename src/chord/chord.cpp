#include <algorithm>
#include "chord.h"

Chord::Chord(int chordPin)
  : chordButton(chordPin) {
  for (int i = 0; i < 8; i++) {
    noteTouch[i] = LOW;
    prevNoteTouch[i] = LOW;
  }

  //setting chord types; tried to do outside of object but it kept throwing mukltiple definition errors
  std::vector<int> major = { 0, 7, 4 };
  std::vector<int> minor = { 0, 7, 3 };
  std::vector<int> augmented = { 0, 8, 4 };
  std::vector<int> diminished = { 0, 6, 3 };
  std::vector<int> majorSeven = { 0, 7, 4, 11 };
  std::vector<int> minorSeven = { 0, 7, 3, 10 };
  std::vector<int> dominantSeven = { 0, 7, 4, 10 };
  std::vector<int> halfDiminishedSeven = { 0, 6, 3, 10 };
  std::vector<int> fullDiminishedSeven = { 0, 6, 3, 9 };
  noteMap[MAJOR] = major;
  noteMap[MINOR] = minor;
  noteMap[AUGMENTED] = augmented;
  noteMap[DIMINISHED] = diminished;
  noteMap[MAJORSEVEN] = majorSeven;
  noteMap[MINORSEVEN] = minorSeven;
  noteMap[DOMINANTSEVEN] = dominantSeven;
  noteMap[HALFDIMINISHEDSEVEN] = halfDiminishedSeven;
  noteMap[FULLDIMINISHEDSEVEN] = fullDiminishedSeven;
}

void Chord::initRoot(int root_) {
  if ((root_ % 12) < 7) {
    this->root = (root_ % 12) + 48;
  } else {
    this->root = (root_ % 12) + 36;
  }
}

void Chord::setRoot(int root_) {
  if ((root_ % 12) < 7) {
    this->root = (root_ % 12) + 48;
  } else {
    this->root = (root_ % 12) + 36;
  }
  populateVector();
}

int Chord::getRoot() {
  return this->root;
}

void Chord::setChordType(chordTypes type_) {
  this->type = type_;
  populateVector();
}

chordTypes Chord::getChordType() {
  return this->type;
}

void Chord::populateVector() {
  possibleNotes.clear();
  for (unsigned int i = 0; i < noteMap[type].size(); i++) {
    possibleNotes.push_back(root + noteMap[type].at(i));
  }
  strumPitches.clear();
  while (strumPitches.size() < 8) {
    int note = possibleNotes.at(strumPitches.size() % possibleNotes.size()) - 132;
    while (true) {      //will make duplicates can sort out after
      if (note < 60) {  //if less than 60
        note += 12;
      } else if (note > 96) {
        note -= 36;
      } else if (std::find(strumPitches.begin(), strumPitches.end(), note) != strumPitches.end()) {  //if it's already in the array
        note += 12;
      } else {
        break;  //break out of loop if all conditions are met
      }
    }
    strumPitches.push_back(note);
  }
  if (possibleNotes.size() == 3) {
    //patterns are none repeating so they are hardcoded for now (may need to think of ways to voice chords better)
    sustainPitches[0] = possibleNotes.at(0);       // root
    sustainPitches[1] = possibleNotes.at(1);       // fifth
    sustainPitches[2] = possibleNotes.at(2) + 12;  // third
  } else {
    sustainPitches[0] = possibleNotes.at(0);       // root
    sustainPitches[1] = possibleNotes.at(3);       // seventh
    sustainPitches[2] = possibleNotes.at(2) + 12;  // third
    strumPitches.at(1) += 24;
    strumPitches.at(2) += 24;
  }
  std::sort(strumPitches.begin(), strumPitches.end());
  std::reverse(strumPitches.begin(), strumPitches.end());
}

void Chord::noteOnHandler(void (*func)(int, int, bool)) {
  noteOn = *func;
}

void Chord::noteOffHandler(void (*func)(int, int, bool)) {
  noteOff = *func;
}

void Chord::capCheckHandler(void (*func)(int*)) {
  capCheck = *func;
}

void Chord::chordPress() {
  for (int i = 0; i < 3; i++) {
    noteOn(i, sustainPitches[i], SUSTAIN);
  }
}

void Chord::chordRelease() {
  for (int i = 0; i < 3; i++) {
    noteOff(i, sustainPitches[i], SUSTAIN);
  }
  for (int i = 0; i < 8; i++) {
    noteOff(i, strumPitches[i], STRUM);
  }
}

void Chord::chordSustain() {
  capCheck(noteTouch);
  for (int i = 0; i < 8; i++) {
    if (noteTouch[i] == HIGH && prevNoteTouch[i] == LOW) {
      noteOn(i, strumPitches[i], STRUM);
    } else if (noteTouch[i] == LOW && prevNoteTouch[i] == HIGH) {
      noteOff(i, strumPitches[i], STRUM);
    }
    prevNoteTouch[i] = noteTouch[i];
  }
}

void Chord::update() {
  chordButton.update();
  switch (chordButton.buttonCheck()) {
    case 1:
      chordPress();
      break;
    case 2:
      chordSustain();
      break;
    case 3:
      chordRelease();
      break;
  }
}

Button* Chord::getButton() {
  return &chordButton;
}