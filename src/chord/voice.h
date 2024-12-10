#ifndef voice_h
#define voice_h

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

class Voice {
public:
  AudioSynthWaveform              waveform;
  AudioEffectEnvelope             envelope;
  AudioAnalyzeRMS                 peak;
  AudioConnection                 *patchCord[2]; // total patchCordCount:1 including array typed ones.

  Voice(int waveform_, float amplitude_);
  void setEnvelope(float attack, float decay, float sustain, float release);
  void setWaveform(int waveform_, float amplitude_);
  void noteOn(int note);
  void noteOff(int note);
  bool isPlaying();
  float readPeak();
};

#endif 