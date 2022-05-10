#ifndef _ADSR_H_
#define _ADSR_H_

#include "definitions.h"

#include <math.h>
#include <complex.h>
#include <iostream>

class Adsr
{
public:
  Adsr();
  ~Adsr();

  void envelopeGen(int velocity,float a,float d,float s,float r,float *output);

  double  attackCount;
  double  decayCount;
  double  releaseCount;

  float attack;
  float decay;
  float sustain;
  float release;

  float current;

private:

};

#endif
