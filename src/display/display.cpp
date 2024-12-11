#include "display.h"

Display::Display()
{
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (1)
            ; // Don't proceed, loop forever
    }
    Serial.println("SSD1306 allocation success!");
    display->clearDisplay();
    display->display();
}

void Display::setChords(Chord** _chords) {
    chords = _chords;
}
void Display::setStrumVoices(Voice** _strumVoices) {
    strumVoices = _strumVoices;
}

void Display::setEditMode(EncoderEditable** _editMode) {
    editMode = _editMode;
}

void Display::setVolume(Volume* _volume) {
    volume = _volume;
}
void Display::setVelocity(Velocity* _velocity) {
    velocity = _velocity;
}
void Display::setEditSelector(int* _editSelector) {
    editSelector = _editSelector;
}

void Display::displayUI()
{
    display->clearDisplay();
    display->setTextColor(WHITE);
    displayLabel("Chord Root:", 0);
    displayItem(getChordRoot(), 0);
    displayLabel("Chord Type:", 1);
    displayItem(getChordType(), 1);
    displayLabel("Volume:", 2);
    displayItem(getVolume(), 2);
    displayLabel("Velocity:", 3);
    displayItem(String(velocity->getVelocity()), 3);
    displaySelector((*editMode)->getNum());
    display->display();
}

void Display::animateStrings()
{
    if (animationTime >= 60)
    { // slow down to 24 fps so that it is less laggy
        display->clearDisplay();
        for (int i = 0; i < 8; i++)
        {
            int yLevel = (i * 8) + 4;
            float jitterAmount = strumVoices[i]->readPeak() * 4;
            float prevJitter = 0;
            for (int j = 0; j < display->width(); j += 8)
            {
                int randomInt = (j < SCREEN_WIDTH - 8) ? random(-4, 5) : 0;
                float jitter = randomInt * jitterAmount;
                display->drawLine(j, yLevel + prevJitter, j + 8, yLevel + jitter, WHITE);
                prevJitter = jitter;
            }
        }
        display->display();
        animationTime = 0;
    }
}

void Display::displayLabel(String labelText, int position)
{
    display->setTextSize(1);
    display->setTextColor(WHITE);
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(labelText, 0, 0, &x1, &y1, &w, &h);
    if (position == 0)
        display->setCursor(0, 0);
    else if (position == 1)
        display->setCursor(0, SCREEN_HEIGHT / 2);
    else if (position == 2)
        display->setCursor(SCREEN_WIDTH - w, 0);
    else if (position == 3)
        display->setCursor(SCREEN_WIDTH - w, SCREEN_HEIGHT / 2);
    display->print(labelText);
}

void Display::displayItem(String itemText, int position)
{
    display->setTextSize(2);
    display->setTextColor(WHITE);
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(itemText, 0, 0, &x1, &y1, &w, &h);
    if (position == 0)
        display->setCursor(0, 10);
    else if (position == 1)
        display->setCursor(0, (SCREEN_HEIGHT / 2) + 10);
    else if (position == 2)
        display->setCursor(SCREEN_WIDTH - w, 10);
    else if (position == 3)
        display->setCursor(SCREEN_WIDTH - w, (SCREEN_HEIGHT / 2) + 10);
    display->print(itemText);
}

void Display::displaySelector(int position)
{
    display->setTextSize(2);
    display->setTextColor(WHITE);
    if (position == 0)
        display->setCursor(SCREEN_WIDTH / 2, 10);
    else if (position == 1)
        display->setCursor(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + 10);
    else if (position == 2)
        display->setCursor(SCREEN_WIDTH / 2, 10);
    else if (position == 3)
        display->setCursor(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + 10);

    if (position < 2)
        display->print(F("<"));
    else
        display->print(F(">"));
}

String Display::getChordRoot()
{
    int root = abs(chords[*editSelector]->getRoot() % 12);
    if (root == 0) return "C";
    else if (root == 1) return "C#/Db";
    else if (root == 2) return "D";
    else if (root == 3) return "D#/Eb";
    else if (root == 4) return "E";
    else if (root == 5) return "F";
    else if (root == 6) return "F#/Gb";
    else if (root == 7) return "G";
    else if (root == 8) return "G#/Ab";
    else if (root == 9) return "A";
    else if (root == 10) return "A#/Bb";
    else if (root == 11) return "B";
    else return "";
}

String Display::getChordType()
{
    chordTypes type = chords[*editSelector]->getChordType();
    if (type == MAJOR)                      return "Maj";
    else if (type == MINOR)                 return "min";
    else if (type == AUGMENTED)             return "aug";
    else if (type == DIMINISHED)            return "dim";
    else if (type == MAJORSEVEN)            return "Maj7";
    else if (type == MINORSEVEN)            return "min7";
    else if (type == DOMINANTSEVEN)         return "dom7";
    else if (type == HALFDIMINISHEDSEVEN)   return "m7b5";
    else if (type == FULLDIMINISHEDSEVEN)   return "dim7";
    else return "";
}

String Display::getVolume()
{
    String volStr = String(volume->getVolume(), 2);
    return volStr;
}