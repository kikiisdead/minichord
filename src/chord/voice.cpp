#include "voice.h"

Voice::Voice(int waveform_, float amplitude_) { 
  int pci = 0; // used only for adding new patchcords
  patchCord[pci++] = new AudioConnection(waveform, 0, envelope, 0); 
  patchCord[pci++] = new AudioConnection(envelope, 0, peak, 0);
  waveform.begin(waveform_);
  waveform.amplitude(amplitude_);
}

void Voice::setEnvelope(float attack, float decay, float sustain, float release) {
  envelope.attack(attack);
  envelope.decay(decay);
  envelope.sustain(sustain);
  envelope.release(release);
}

void Voice::setWaveform(int waveform_, float amplitude_) {
  waveform.begin(waveform_);
  waveform.amplitude(amplitude_);
}

void Voice::noteOn(int note) {
  float freq = 440 * pow(2.0, (float)(note - 69) / (float)12);
  waveform.frequency(freq);
  envelope.noteOn();
}

void Voice::noteOff(int note) {
  envelope.noteOff();
}

bool Voice::isPlaying() {
  return envelope.isActive();
}

float Voice::readPeak() {
  return peak.read();
}