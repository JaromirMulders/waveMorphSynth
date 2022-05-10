#include "oscillator.h"

Oscillator::Oscillator() //constructor
{
  for(int i = 0; i < cTableSize; i++){ //fill waveTable with sinewave
    waveTable[i] = sin(i/(float)cTableSize * TWOPI);
  }

  fqCounter = 0;
  index1 = 0;
  index2 = 0;
  phasor = 0;
  frac   = 0;

  filter = 0;
  oldVal1 = 0;
  oldVal2 = 0;

}

float clip(float input,float lowVal,float highVal)
{
  if(input > highVal){
    input = highVal;
  } else
  if(input < lowVal){
    input = lowVal;
  }

  return input;

}

float Oscillator::toSaw(float input,float shape,float schapeScale){
  input =
    clip(
       (asinh(
         sin(
           pow(input,shape) * TWOPI)
              )
        )
       ,(((float)1 - shapeScale) * -1) ,1)
    + ((shapeScale * 0.5) - shapeScale) * (shapeScale + (float)1);

  return input;
}

float Oscillator::toSquare(float input,float square){
  input = tanh(input * square);

  return input;
}

float Oscillator::toTimbre(float input,float timbre){
  input =
    tanh(
      (sin(
      tanh(input * (timbre + (float)1))
       * (timbre * (((float)1-timbre)) * (float)10 + 0.1) * (float)9
       + (timbre + 0.25)))
       * (timbre * (float)5 + 1));

  return input;
}


void Oscillator::makeTable(float shape, float square, float timbre)
{
  if(oldVal1 != shape || oldVal2 != square || oldVal3 != timbre){ //if values change
    shapeScale = (shape - (float)1) * 0.11;

    for(int i = 0; i < cTableSize; i++)
    { //fill waveTable with sinewave
      waveTable[i] = (i/(float)cTableSize); //sawtooth
      waveTable[i] = toSaw(waveTable[i],shape,shapeScale);
      waveTable[i] = toSquare(waveTable[i],square);
      waveTable[i] = toTimbre(waveTable[i],timbre);

    }
  }

oldVal1 = shape;
oldVal2 = square;
oldVal3 = timbre;
}


void Oscillator::oscGen(float *fq,float *output)
{

    for(int i = 0; i < FRAMESIZE; i++){

      frac = (float)cTableSize * fq[i] / (float)SAMPLERATE; //calculate frequency

      if(fqCounter > cTableSize){ //wrap around if outside buffer
        fqCounter = fqCounter - (double)cTableSize;
      }

      index1 = (int)fqCounter;
      index2 = index1 + 1; //lookup of index
      if(index2 > cTableSize - 1){ //wrap around if outside buffer
        index2 = 0;
      }

      phasor = fqCounter - (double)index1; //getting phase

      //linear interpolation
      wave = (waveTable[index1] * ((double)1 - phasor)) + (waveTable[index2] * phasor);

      fqCounter += frac;

      output[i] = wave;

    }

}


Oscillator::~Oscillator()
{
}
