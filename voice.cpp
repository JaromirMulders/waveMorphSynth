#include "voice.h"

Voice::Voice(){

}

void Voice::generateVoice(
  float complexOscFreq, float fm, float volume1, float lowPassFreq1,
  float shape1, float square1, float timbre,
  float oscillator2Freq, float volume2, float lowPassFreq2,
  float shape2, float square2,
  float ringMod,
  int velocity, float attack, float decay, float sustain, float release,
  float *output){

  complexOsc.oscGen(fmMod,complexOscOut);
  oscillator2.oscGen(fmMod2,oscillator2Out);

  //put oscillators trough low pass gate
  lowPassGate1.filter(volume1,lowPassFreq1,envelope1Out,complexOscOut,complexOscOutLeveled);
  lowPassGate2.filter(volume2,lowPassFreq2,envelope1Out,oscillator2Out,oscillator2OutLeveled);

  //generate enevelope generator
  adsr1.envelopeGen(velocity,attack,decay,sustain,release,envelope1Out);


  //write to output
  for(int i = 0; i < FRAMESIZE; i++){
    //fmModulation
    fmMod[i] = fabs(complexOscFreq + oscillator2Freq * fm * oscillator2Out[i] * fm);
    fmMod2[i] = oscillator2Freq;


    //output
    output[i] =
      ((complexOscOutLeveled[i] + oscillator2OutLeveled[i]) +
      (complexOscOut[i] * oscillator2Out[i] * ringMod * envelope1Out[i])) * 0.25;
    }

}


Voice::~Voice(){
}
