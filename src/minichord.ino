#include "chord/chord.h"
#include "userInterface/betterEncoder.h"
#include "chord/voice.h"
#include "audio/mixers.h"
#include "userInterface/velocity.h"
#include "userInterface/volume.h"
#include "userInterface/chordType.h"
#include "userInterface/chordRoot.h"
#include "userInterface/animation.h"
#include "display/display.h"
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
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

Voice *voices[11] = {&sustainVoice1, &sustainVoice2, &sustainVoice3, &strumVoice1, &strumVoice2, &strumVoice3, &strumVoice4, &strumVoice5, &strumVoice6, &strumVoice7, &strumVoice8};

AudioMixer11 mixer;
AudioOutputI2S i2sOut;
AudioControlSGTL5000 audioShield;
AudioConnection *patchCords[11];
AudioConnection patchCord1(mixer, 0, i2sOut, 0);
AudioConnection patchCord2(mixer, 0, i2sOut, 1);

Chord chord1(CHORDPIN1);
Chord chord2(CHORDPIN2);
Chord chord3(CHORDPIN3);
Chord chord4(CHORDPIN4);
Chord chord5(CHORDPIN5);
Chord chord6(CHORDPIN6);
Chord chord7(CHORDPIN7);

Voice *sustainVoices[3] = {&sustainVoice1, &sustainVoice2, &sustainVoice3};
Voice *strumVoices[8] = {&strumVoice1, &strumVoice2, &strumVoice3, &strumVoice4, &strumVoice5, &strumVoice6, &strumVoice7, &strumVoice8};

Chord *chords[7] = {&chord1, &chord2, &chord3, &chord4, &chord5, &chord6, &chord7};
Button editButton(EDITPIN);

int editSelector = 0;

Velocity velocity(90);
Volume volume(0.5);
ChordType chordType(chords[editSelector]);
ChordRoot chordRoot(chords[editSelector]);
Animation animation;
EncoderEditable *editMode = &chordRoot; // using pointer so as not to create new object, just point to already existing object

BetterEncoder enc(25, 24, 4);

elapsedMillis holdTime;

Display* display;

void setup()
{
  Serial.begin(9600);
  enc.incrementHandler(encoderIncrement);
  enc.decrementHandler(encoderDecrement);

  // capacitive touch sensor
  if (!cap.begin())
  {
    Serial.println("CAP1188 not found");
    while (1)
      ;
  }
  Serial.println("CAP1188 found!");

  // display things 
  display = new Display();
  display->setChords(chords);
  display->setStrumVoices(strumVoices);
  display->setVelocity(&velocity);
  display->setVolume(&volume);
  display->setEditMode(&editMode); 
  display->setEditSelector(&editSelector);

  // chord objects
  for (int i = 0; i < 7; i++)
  {
    chords[i]->noteOnHandler(noteOn);
    chords[i]->noteOffHandler(noteOff);
    chords[i]->capCheckHandler(adaCapCheck);
    chords[i]->initRoot(EEPROM.read(i));
    chords[i]->setChordType((chordTypes)(EEPROM.read(i + 10)));
  }

  // audio things
  for (int i = 0; i < 11; i++)
  {
    patchCords[i] = new AudioConnection(voices[i]->envelope, 0, mixer, i);
    mixer.gain(i, 0.5);
  }

  AudioMemory(12);
  audioShield.enable();
  audioShield.volume(volume.getVolume());

  for (int i = 0; i < 3; i++)
  {
    sustainVoices[i]->setEnvelope(10, 200, 0.6, 80);
  }
  for (int i = 0; i < 8; i++)
  {
    strumVoices[i]->setEnvelope(10, 250, 0.3, 500);
  }

  // everything works if it hits this point
  Serial.println("I'm working");
  display->displayUI();
}

void loop()
{
  enc.process();
  checkEdit();
  checkChordButtons();
  for (int i = 0; i < 7; i++)
  {
    chords[i]->update();
  }
  if (editMode->getNum() == ANIM)
  {
    display->animateStrings();
  }
}

void noteOn(int voice, int noteValue, bool selector)
{ // going into chord object
  usbMIDI.sendNoteOn(noteValue, velocity.getVelocity(), 1);
  if (selector == SUSTAIN)
    sustainVoices[voice]->noteOn(noteValue);
  else
    strumVoices[voice]->noteOn(noteValue);
}

void noteOff(int voice, int noteValue, bool selector)
{ // going into chord object
  usbMIDI.sendNoteOff(noteValue, 0, 1);
  if (selector == SUSTAIN)
    sustainVoices[voice]->noteOff(noteValue);
  else
    strumVoices[voice]->noteOff(noteValue);
}

void adaCapCheck(int noteTouch[8])
{ // going into chord object
  uint8_t touched = cap.touched();
  for (uint8_t i = 0; i < 8; i++)
  {
    if (touched & (1 << i))
      noteTouch[i] = HIGH; // simply returning the position that it is touched/which cap is being touched. returns whichever is firt so only one touch position;
    else
      noteTouch[i] = LOW;
  }
}

void encoderIncrement()
{ // going into BetterEncoder Object
  chordRoot.setChord(chords[editSelector]);
  chordType.setChord(chords[editSelector]);
  editMode->increment(); // using abstraction through a superclass to call this member function
  audioShield.volume(volume.getVolume());
  if (editMode->getNum() != ANIM)
  {
    display->displayUI();
  }
}

void encoderDecrement()
{ // going into BetterEncoder Object
  chordRoot.setChord(chords[editSelector]);
  chordType.setChord(chords[editSelector]);
  editMode->decrement(); // using abstraction through a superclass to call this member function
  audioShield.volume(volume.getVolume());
  if (editMode->getNum() != ANIM)
  {
    display->displayUI();
  }
}

void checkChordButtons()
{
  for (int i = 0; i < 7; i++)
  {
    if (chords[i]->getButton()->buttonCheck() == 1)
    {
      editSelector = i;
      if (chords[editSelector]->getRoot() != EEPROM.read(editSelector))
      {
        EEPROM.write(editSelector, chords[editSelector]->getRoot());
        Serial.println("Writing root note");
      }
      if ((int)chords[editSelector]->getChordType() != EEPROM.read(editSelector + 10))
      {
        EEPROM.write(editSelector + 10, (int)chords[editSelector]->getChordType());
        Serial.println("Writing chord type");
      }
      if (editMode->getNum() != ANIM)
      {
        display->displayUI();
      }
    }
  }
}

void checkEdit()
{
  editButton.update();
  if (editButton.buttonCheck() == 1)
  {
    int edit = editMode->getNum();
    edit = (edit < 3) ? edit + 1 : 0;
    if (edit == ROOTEDIT)
      editMode = &chordRoot;
    else if (edit == CHORDEDIT)
      editMode = &chordType;
    else if (edit == VOLEDIT)
      editMode = &volume;
    else if (edit == VELEDIT)
      editMode = &velocity;
    holdTime = 0;
    display->displayUI();
  }
  if (editButton.buttonCheck() == 2 && holdTime > 500)
  {
    editMode = &animation;
  }
}