#include "chord.h"
#include "betterEncoder.h"
#include "voice.h"
#include "mixers.h"
#include "velocity.h"
#include "volume.h"
#include "chordType.h"
#include "chordRoot.h"
#include "animation.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_CAP1188.h>
#include <string.h>
#include <EEPROM.h>

// MEMORY ALLOCATION
// Addresses 0-6 are for the root note
// Addresses 10-16 are for the chord type
// don't want to write too many times so only write when changing off of the corresponding editMode

#define CHORDPIN1 33
#define CHORDPIN2 34
#define CHORDPIN3 35
#define CHORDPIN4 36
#define CHORDPIN5 37
#define CHORDPIN6 38
#define CHORDPIN7 39
#define EDITPIN 26

#define ROOTEDIT 0
#define CHORDEDIT 1
#define VOLEDIT 2
#define VELEDIT 3
#define ANIM 4

#define CAP1188_RESET -1

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_CAP1188 cap = Adafruit_CAP1188();

Voice sustainVoice1(WAVEFORM_TRIANGLE, 0.5);
Voice sustainVoice2(WAVEFORM_TRIANGLE, 0.5);
Voice sustainVoice3(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice1(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice2(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice3(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice4(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice5(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice6(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice7(WAVEFORM_TRIANGLE, 0.5);
Voice strumVoice8(WAVEFORM_TRIANGLE, 0.5);

Voice* voices[11] = { &sustainVoice1, &sustainVoice2, &sustainVoice3, &strumVoice1, &strumVoice2, &strumVoice3, &strumVoice4, &strumVoice5, &strumVoice6, &strumVoice7, &strumVoice8 };

AudioMixer11 mixer;
AudioOutputI2S i2sOut;
AudioControlSGTL5000 audioShield;
AudioConnection* patchCords[11];
AudioConnection patchCord1(mixer, 0, i2sOut, 0);
AudioConnection patchCord2(mixer, 0, i2sOut, 1);

Chord chord1(CHORDPIN1);
Chord chord2(CHORDPIN2);
Chord chord3(CHORDPIN3);
Chord chord4(CHORDPIN4);
Chord chord5(CHORDPIN5);
Chord chord6(CHORDPIN6);
Chord chord7(CHORDPIN7);

Voice* sustainVoices[3] = { &sustainVoice1, &sustainVoice2, &sustainVoice3 };
Voice* strumVoices[8] = { &strumVoice1, &strumVoice2, &strumVoice3, &strumVoice4, &strumVoice5, &strumVoice6, &strumVoice7, &strumVoice8 };

Chord* chords[7] = { &chord1, &chord2, &chord3, &chord4, &chord5, &chord6, &chord7 };
Button editButton(EDITPIN);

int editSelector = 0;

Velocity velocity(90);
Volume volume(0.5);
ChordType chordType(chords[editSelector]);
ChordRoot chordRoot(chords[editSelector]);
Animation animation;
EncoderEditable* editMode = &chordRoot; //using pointer so as not to create new object, just point to already existing object

BetterEncoder enc(25, 24, 4);

elapsedMillis holdTime;
elapsedMillis animationTime;

void setup() {
  Serial.begin(9600);
  enc.incrementHandler(encoderIncrement);
  enc.decrementHandler(encoderDecrement);

  //capacitive touch sensor
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1)
      ;
  }
  Serial.println("CAP1188 found!");

  //chord objects
  for (int i = 0; i < 7; i++) {
    chords[i]->noteOnHandler(noteOn);
    chords[i]->noteOffHandler(noteOff);
    chords[i]->capCheckHandler(adaCapCheck);
    chords[i]->initRoot(EEPROM.read(i));
    chords[i]->setChordType((chordTypes)(EEPROM.read(i + 10)));
  }

  //display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1)
      ;  // Don't proceed, loop forever
  }
  Serial.println("SSD1306 allocation success!");
  display.clearDisplay();
  displayUI();
  display.display();

  //audio things
  for (int i = 0; i < 11; i++) {
    patchCords[i] = new AudioConnection(voices[i]->envelope, 0, mixer, i);
    mixer.gain(i, 0.5);
  }

  AudioMemory(12);
  audioShield.enable();
  audioShield.volume(volume.getVolume());

  for (int i = 0; i < 3; i++) {
    sustainVoices[i]->setEnvelope(10, 200, 0.6, 80);
  }
  for (int i = 0; i < 8; i++) {
    strumVoices[i]->setEnvelope(10, 250, 0.3, 500);
  }

  //everything works if it hits this point
  Serial.println("I'm working");
}

void loop() {
  enc.process();
  checkEdit();
  checkChordButtons();
  for (int i = 0; i < 7; i++) {
    chords[i]->update();
  }
  if (editMode->getNum() == ANIM) {
    animateStrings();
  }
}

void noteOn(int voice, int noteValue, bool selector) { //going into chord object
  usbMIDI.sendNoteOn(noteValue, velocity.getVelocity(), 1);
  if (selector == SUSTAIN) sustainVoices[voice]->noteOn(noteValue);
  else strumVoices[voice]->noteOn(noteValue);
}

void noteOff(int voice, int noteValue, bool selector) { //going into chord object
  usbMIDI.sendNoteOff(noteValue, 0, 1);
  if (selector == SUSTAIN) sustainVoices[voice]->noteOff(noteValue);
  else strumVoices[voice]->noteOff(noteValue);
}

void adaCapCheck(int noteTouch[8]) { // going into chord object
  uint8_t touched = cap.touched();
  for (uint8_t i = 0; i < 8; i++) {
    if (touched & (1 << i)) noteTouch[i] = HIGH;  //simply returning the position that it is touched/which cap is being touched. returns whichever is firt so only one touch position;
    else noteTouch[i] = LOW;
  }
}

void encoderIncrement() { // going into BetterEncoder Object
  chordRoot.setChord(chords[editSelector]);
  chordType.setChord(chords[editSelector]);
  editMode->increment(); // using abstraction through a superclass to call this member function
  audioShield.volume(volume.getVolume());
  if (editMode->getNum() != ANIM) {
    displayUI();
  }
}

void encoderDecrement() { // going into BetterEncoder Object
  chordRoot.setChord(chords[editSelector]);
  chordType.setChord(chords[editSelector]);
  editMode->decrement(); // using abstraction through a superclass to call this member function
  audioShield.volume(volume.getVolume());
  if (editMode->getNum() != ANIM) {
    displayUI();
  }
}

void checkChordButtons() {
  for (int i = 0; i < 7; i ++) {
    if (chords[i]->getButton()->buttonCheck() == 1) {
      editSelector = i;
      if (chords[editSelector]->getRoot() != EEPROM.read(editSelector)) {
        EEPROM.write(editSelector, chords[editSelector]->getRoot());
        Serial.println("Writing root note");
      }
      if ((int)chords[editSelector]->getChordType() != EEPROM.read(editSelector + 10)) {
        EEPROM.write(editSelector + 10, (int)chords[editSelector]->getChordType());
        Serial.println("Writing chord type");
      }
      if (editMode->getNum() != ANIM) {
        displayUI();
      }
    }
  }
}

void checkEdit() {
  editButton.update();
  if (editButton.buttonCheck() == 1) {
    int edit = editMode->getNum();
    edit = (edit < 3) ? edit + 1 : 0;
    if (edit == ROOTEDIT)       editMode = &chordRoot;
    else if (edit == CHORDEDIT) editMode = &chordType;
    else if (edit == VOLEDIT)   editMode = &volume;
    else if (edit == VELEDIT)   editMode = &velocity;
    holdTime = 0;
    displayUI();
  }
  if (editButton.buttonCheck() == 2 && holdTime > 500) {
    editMode = &animation;
  }
}

//UI THINGS
void displayUI() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  displayLabel("Chord Root:", 0);
  displayItem(getChordRoot(), 0);
  displayLabel("Chord Type:", 1);
  displayItem(getChordType(), 1);
  displayLabel("Volume:", 2);
  displayItem(getVolume(), 2);
  displayLabel("Velocity:", 3);
  displayItem(String(velocity.getVelocity()), 3);
  displaySelector(editMode->getNum());
  display.display();
}

void animateStrings() {
  if (animationTime >= 60) {  //slow down to 24 fps so that it is less laggy
    display.clearDisplay();
    for (int i = 0; i < 8; i++) {
      int yLevel = (i * 8) + 4;
      float jitterAmount = strumVoices[i]->readPeak() * 4;
      float prevJitter = 0;
      for (int j = 0; j < display.width(); j += 8) {
        int randomInt = (j < SCREEN_WIDTH - 8) ? random(-4, 5) : 0;
        float jitter = randomInt * jitterAmount;
        display.drawLine(j, yLevel + prevJitter, j + 8, yLevel + jitter, WHITE);
        prevJitter = jitter;
      }
    }
    display.display();
    animationTime = 0;
  }
}

void displayLabel(String labelText, int position) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(labelText, 0, 0, &x1, &y1, &w, &h);
  if (position == 0)      display.setCursor(0, 0);
  else if (position == 1) display.setCursor(0, SCREEN_HEIGHT / 2);
  else if (position == 2) display.setCursor(SCREEN_WIDTH - w, 0);
  else if (position == 3) display.setCursor(SCREEN_WIDTH - w, SCREEN_HEIGHT / 2);
  display.print(labelText);
}

void displayItem(String itemText, int position) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(itemText, 0, 0, &x1, &y1, &w, &h);
  if (position == 0)      display.setCursor(0, 10);
  else if (position == 1) display.setCursor(0, (SCREEN_HEIGHT / 2) + 10);
  else if (position == 2) display.setCursor(SCREEN_WIDTH - w, 10);
  else if (position == 3) display.setCursor(SCREEN_WIDTH - w, (SCREEN_HEIGHT / 2) + 10);
  display.print(itemText);
}

void displaySelector(int position) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if (position == 0)      display.setCursor(SCREEN_WIDTH / 2, 10);
  else if (position == 1) display.setCursor(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + 10);
  else if (position == 2) display.setCursor(SCREEN_WIDTH / 2, 10);
  else if (position == 3) display.setCursor(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + 10);

  if (position < 2) display.print(F("<"));
  else display.print(F(">"));
}

String getChordRoot() {
  int root = abs(chords[editSelector]->getRoot() % 12);
  if (root == 0)        return "C";
  else if (root == 1)   return "C#/Db";
  else if (root == 2)   return "D";
  else if (root == 3)   return "D#/Eb";
  else if (root == 4)   return "E";
  else if (root == 5)   return "F";
  else if (root == 6)   return "F#/Gb";
  else if (root == 7)   return "G";
  else if (root == 8)   return "G#/Ab";
  else if (root == 9)   return "A";
  else if (root == 10)  return "A#/Bb";
  else if (root == 11)  return "B";
  else                  return "";
}

String getChordType() {
  chordTypes type = chords[editSelector]->getChordType();
  if (type == MAJOR)                    return "Maj";
  else if (type == MINOR)               return "min";
  else if (type == AUGMENTED)           return "aug";
  else if (type == DIMINISHED)          return "dim";
  else if (type == MAJORSEVEN)          return "Maj7";
  else if (type == MINORSEVEN)          return "min7";
  else if (type == DOMINANTSEVEN)       return "dom7";
  else if (type == HALFDIMINISHEDSEVEN) return "m7b5";
  else if (type == FULLDIMINISHEDSEVEN) return "dim7";
  else                                  return "";
}

String getVolume() {
  String volStr = String(volume.getVolume(), 2);
  return volStr;
}