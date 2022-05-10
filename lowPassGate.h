#ifndef _LOWPASSGATE_H_
#define _LOWPASSGATE_H_

#include "definitions.h"

#include <math.h>
#include <complex.h>
#include <iostream>

class LowPassGate
{
public:
  LowPassGate();
  ~LowPassGate();

  float clip(float input, float lowVal, float HighVal);
  void filter(float level,float frequency,float *envelope,float *input,float *output);

private:

};

#endif
