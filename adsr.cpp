#include "adsr.h"

Adsr::Adsr()
{
  attackCount  = 0;
  decayCount   = 0;
  releaseCount = 0;

}

void Adsr::envelopeGen(int velocity,float a,float d,float s,float r,float *output)
{
  attack  = (float)1000 / (float)a;
  decay   = (float)1000 / (float)d;
  sustain = s;
  release = (float)1000 / (float)r;

  for(int i = 0; i < FRAMESIZE; i++){

    if(velocity > 0){
    	releaseCount = 0;

    	//attack
    	attackCount = attackCount + attack / SAMPLERATE; //ramp up
    	output[i] = pow(attackCount,0.75);
    	//decay
    	if(attackCount > 1){
    		decayCount = decayCount + decay / SAMPLERATE; //ramp up
    		output[i] = ((1 - decayCount) * (1 - sustain)) + sustain;
    		//sustain
    		if(decayCount > 1){
    			output[i] = sustain;
    		}//if
    	}//if
    }else{
    	attackCount = 0;
    	decayCount = 0;

    	//release
    	releaseCount = releaseCount + release / SAMPLERATE; //ramp up
    	output[i] = (1 - pow(releaseCount,0.7)) * sustain;

    	if(releaseCount > 1){
    		output[i] = 0;

    	}//if
    }//else

  }//for


}//envelopeGen



Adsr::~Adsr(){


}
