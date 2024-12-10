#ifndef display_h
#define display_h

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "chord/chord.h"
#include "userInterface/betterEncoder.h"
#include "chord/voice.h"
#include "audio/mixers.h"
#include "userInterface/velocity.h"
#include "userInterface/volume.h"
#include "userInterface/chordType.h"
#include "userInterface/chordRoot.h"
#include "userInterface/animation.h"
#include "interfaces/encoderEditable.h"

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

class Display {
public:
    Display();
    Adafruit_SSD1306* display;
    void displayUI();
    void animateStrings();
    void setEditMode(EncoderEditable** _editMode);
    void setVolume(Volume* _volume);
    void setVelocity(Velocity* _velocity);
    void setEditSelector(int* _editSelector);
    void setChords(Chord** _chords);
    void setStrumVoices(Voice** _strumVoices);
private:
    elapsedMillis animationTime;
    Chord** chords;
    Voice** strumVoices;
    Volume* volume;
    Velocity* velocity;
    EncoderEditable** editMode; //pointer to pointer
    int* editSelector;
    void displayLabel(String labelText, int position);
    void displayItem(String itemText, int position);
    void displaySelector(int position);
    String getChordRoot();
    String getChordType();
    String getVolume();

};

#endif