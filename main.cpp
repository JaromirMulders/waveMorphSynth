#include "oscillator.h"
#include "adsr.h"
#include "lowPassGate.h"
#include "jackMidi.h"
#include "voice.h"

#include "definitions.h"

#include <iostream>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <cstdlib>

#include <jack/jack.h>

//global variables--------------------------------------------------------------
//controls
float complexOscFreq[VOICEAMNT];
float fm      = 0;
float volume1 = 0.5;
float lowPassFreq1 = 0.15;

float shape1  = 0;
float square1 = 0;
float timbre  = 0;

float oscillator2Freq[VOICEAMNT];
float volume2 = 0;
float lowPassFreq2 = 0.15;

float shape2  = 0;
float square2 = 0;

float ringMod = 0;

int velocityArray[VOICEAMNT];

int   attack = 100;
int   decay  = 100;
float sustain = 0.8;
int   release = 1000;


//ui----------------------------------------------------------------------------
JackMidi jackMidi;

//process-----------------------------------------------------------------------
//audio init
jack_port_t *output_port;

//create five new voices
Voice voice[VOICEAMNT];

float outputMix = 0;

//create five new audio outputs
float **voiceOutput;


//callback
int process(jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *out = //get output
    (jack_default_audio_sample_t *) jack_port_get_buffer(output_port,nframes);

    for(int i = 0; i < VOICEAMNT; i++){ //process voice
      voice[i].generateVoice(
        complexOscFreq[i], fm, volume1, lowPassFreq1,
        shape1, square1, timbre,
        oscillator2Freq[i], volume2, lowPassFreq2,
        shape2, square2,
        ringMod,
        velocityArray[i], attack, decay, sustain, release,
        voiceOutput[i]
      );
    }

    for(int i = 0; i < FRAMESIZE; i++){//send voices to output
      out[i] = (//mix voices together
      voiceOutput[0][i]  +
      voiceOutput[1][i] +
      voiceOutput[2][i] +
      voiceOutput[3][i] +
      voiceOutput[4][i])  * 0.1;
    }
  return 0;
} // process()

//shutdown callback may be called by JACK---------------------------------------
void jack_shutdown(void *arg)
{
  exit(1);
}

//update Samplerate-------------------------------------------------------------
int updatesamplerate(jack_nframes_t nframes, void *arg)
{
  std::cout << "Sample rate set to: " << nframes << std::endl;
  return 0;
}

//main--------------------------------------------------------------------------
int main()
{
  //starting up midi receive----------------------------------------------------
  jackMidi.activate();

  //creating audio outputs------------------------------------------------------
  voiceOutput = new float*[VOICEAMNT];
  for(int i = 0; i < VOICEAMNT; i++){
    voiceOutput[i] = new float[FRAMESIZE];
  }

  //starting up jack------------------------------------------------------------
  // Create a new Jack client
  jack_client_t *client;
  const char **ports;

  if( (client=jack_client_open("jackSynth",(jack_options_t)0,0)) == 0)
  {
    std::cout << "JACK server not running ?\n";
    return 1;
  }

  // Install a shutdown routine
  jack_on_shutdown(client,jack_shutdown,0); // install a shutdown callback

  // Install a routine
  jack_set_sample_rate_callback(client,updatesamplerate,0);

  // Install the sample processing callback
  jack_set_process_callback(client,process,0);


  output_port = jack_port_register(client,"out",
     JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);

  std::cout << "Our output port is called: " << jack_port_name(output_port) << std::endl;

  // Get rollin'
  if(jack_activate(client)){
    std::cout <<  "cannot activate client";
    return 1;
  }

  /*
   * The next calls try to auto-connect to a receiving client
   */
  // See what ports are available to receive our

  if((ports =
  jack_get_ports(client,"system|meter",0,JackPortIsInput)) == 0){
    std::cout << "Cannot find any physical playback ports\n";
    exit(1);
  }


  /*
  // List all ports matching the search criteria
  std::cout << "ports in:" << "\n";
  for(int p=0; ports[p] != 0; p++){
    std::cout << "Ports found: " << ports[p] << std::endl;
  }
  */

  // first output
  if(jack_connect(client,jack_port_name(output_port),ports[0])){
    std::cout << "Cannot connect output ports\n";
  }

  // second output
  if(jack_connect(client,jack_port_name(output_port),ports[1])){
    std::cout << "Cannot connect output ports\n";
  }




  free(ports); // ports structure no longer needed

  while(1){

   jackMidi.getScaledValues
   (
     complexOscFreq, fm, volume1, lowPassFreq1,
     shape1, square1, timbre,
     oscillator2Freq, volume2, lowPassFreq2,
     shape2, square2,
     ringMod,
     velocityArray, attack, decay, sustain, release
   );

   //create wavetables
   for(int i = 0; i < VOICEAMNT; i++){
    voice[i].complexOsc.makeTable(shape1,square1,timbre);
    voice[i].oscillator2.makeTable(shape2,square2,0);
   }

   usleep(1000);

  };

  jack_client_close(client);

  return 1;
}
