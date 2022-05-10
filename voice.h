#ifndef _VOICE_H_
#define _VOICE_H_

#include "oscillator.h"
#include "adsr.h"
#include "lowPassGate.h"


class Voice
{
public:
Voice();
~Voice();

void generateVoice(
  float complexOscFreq, float fm, float volume1, float lowPassFreq1,
  float shape1, float square1, float timbre,
  float oscillator2Freq, float volume2, float lowPassFreq2,
  float shape2, float square2,
  float ringMod,
  int velocity, float attack, float decay, float sustain, float release,
  float *output);

  Oscillator complexOsc; //creating Oscillator
  Oscillator oscillator2;

  Adsr adsr1;

  LowPassGate lowPassGate1;
  LowPassGate lowPassGate2;

  //arrays for sending audio to and from classes
  float fmMod[FRAMESIZE];
  float fmMod2[FRAMESIZE];
  float complexOscOut[FRAMESIZE];
  float oscillator2Out[FRAMESIZE];
  float complexOscOutLeveled[FRAMESIZE];
  float oscillator2OutLeveled[FRAMESIZE];
  float envelope1Out[FRAMESIZE];

private:



};

#endif
