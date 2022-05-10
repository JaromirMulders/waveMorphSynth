#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_

#include "definitions.h"

#include <math.h>
#include <complex.h>
#include <iostream>

class Oscillator
{
public:
  //functions
  Oscillator();
  ~Oscillator();

  void settings();
  void oscGen(float *fq,float *output);
  void makeTable(float shape, float square, float timbre);

private:

  float toSaw(float input,float shape,float schapeScale);
  float toSquare(float input, float square);
  float toTimbre(float input, float timbre);

  float waveTable[cTableSize];
  double fqCounter;
  long   index1;
  long   index2;
  double frac;
  double phasor;
  float wave;
  float shapeScale;

  float filter;
  float oldVal1;
  float oldVal2;
  float oldVal3;

}; //

#endif
