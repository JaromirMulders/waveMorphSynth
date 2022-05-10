#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <math.h>

#include "definitions.h"


class JackMidi
{
public:
  JackMidi();
  ~JackMidi();

  void activate();

  float mtof(int midiIn);
  void calculateCoarse(int coarse,int identifier);
  void calculateFine(int fine,int identifier);
  void calculateFM(int fm, int identifier);
  void calculateLowPassGate(int volume, int identifier);
  void calulateWaveShaperShape(int shape, int identifier);
  void calulateWaveShaperSquare(int square, int identifier);
  void calulateWaveShaperTimbre(int timbre, int identifier);
  void calulateRingmod(int ringMod, int identifier);
  void calulateAttack(int attack, int identifier);
  void calulateDecay(int decay, int identifier);
  void calulateSustain(int sustain, int identifier);
  void calulateRelease(int release, int identifier);

  float scaledParams[POTAMNT];

  void getScaledValues
  (
    float *complexOscFreq, float &fm, float &volume1, float &lowPassFreq1,
    float &shape1, float &square1, float &timbre,
    float *oscillator2Freq, float &volume2, float &lowPassFreq2,
    float &shape2, float &square2,
    float &ringMod,
    int *velocity, int &attack, int &decay, float &sustain, int &release
  );

  void getArrays(int *velocity);

  float complexOscFreq;;
  float fm;
  float volume1;
  float lowPassFreq1;

  float shape1;
  float square1;
  float timbre;

  float oscillator2Freq;
  float volume2;
  float lowPassFreq2;

  float shape2;
  float square2;

  float ringMod;

  int velocity;

  int attack;
  int decay;
  float sustain;
  int release;

private:

  static int staticProcess(jack_nframes_t nframes, void *arg);
  int process(jack_nframes_t nframes);

  void poly(int midiIn, int velocity);

  int noteArray[VOICEAMNT];
  int velocityArray[VOICEAMNT];
  float complexFreqArray[VOICEAMNT];
  float oscillator2FreqArray[VOICEAMNT];

  int pots[POTAMNT];
  int midNote;
  int midVel;

  jack_client_t*  client;
  jack_port_t*    inputPort;

  const char **ports;
  const char **portsOut;

};
