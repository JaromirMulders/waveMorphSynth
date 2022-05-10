#include "lowPassGate.h"

LowPassGate::LowPassGate(){
}

float LowPassGate::clip(float input,float lowVal,float highVal)
{
  if(input > highVal){
    input = highVal;
  } else
  if(input < lowVal){
    input = lowVal;
  }

  return input;
}


void LowPassGate::filter(float level,float frequency,float *envelope,float *input,float *output)
{
  static float sampleDelay;
  static float freq;

  for(int i = 0; i < FRAMESIZE; i++){

    freq = frequency * envelope[i];

    //filter
    output[i] = (input[i] * ((float)1 - freq)) + (sampleDelay * freq);
    sampleDelay = output[i];

    //vca
    output[i] = input[i] * level * envelope[i];
  }

}


LowPassGate::~LowPassGate(){
}
