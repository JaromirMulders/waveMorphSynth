#include "jackMidi.h"

JackMidi::JackMidi(){
  std::cout << "Jack()" << std::flush;

  if ((client = jack_client_open("WaveMorphSynthMidiIn", JackNullOption, NULL)) == 0)
  {
    std::cout << "jack server not running?" << std::endl;
  }


  jack_set_process_callback (client, staticProcess, static_cast<void*>(this));

  inputPort  = jack_port_register (client, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

  /*
   * The next calls try to auto-connect to a receiving client
   */
  // See what ports are available to receive our

  if((ports =
  jack_get_ports(client,"system|meter",0,JackPortIsInput)) == 0){
    std::cout << "Cannot find any physical playback ports\n";
    exit(1);
  }
  if((portsOut =
  jack_get_ports(client,"system|meter",0,JackPortIsOutput)) == 0){
    std::cout << "Cannot find any physical playback ports\n";
    exit(1);
  }

  std::cout << "ports in:" << "\n";
  for(int p=0; ports[p] != 0; p++){
    std::cout << "Ports found: " << ports[p] << std::endl;
  }

  std::cout << "ports out:" << "\n";
  for(int p=0; portsOut[p] != 0; p++){
    std::cout << "Ports found: " << portsOut[p] << std::endl;
  }


  std::cout << "\t\t\tDone!" << std::endl;

  //initialise pots array
  for(int i = 0; i < POTAMNT; i++){
    pots[i] = 0;
  }

  complexOscFreq = 0;
  fm      = 0;
  volume1 = 0.5;
  lowPassFreq1 = 0.15;

  shape1  = 0;
  square1 = 0;
  timbre  = 0;

  oscillator2Freq = 0;
  volume2 = 0;
  lowPassFreq2 = 0.15;

  shape2  = 0;
  square2 = 0;

  ringMod = 0;

  velocity = 100;

  attack = 100;
  decay  = 100;
  sustain = 0.8;
  release = 1000;

  midNote = 48;
  midVel = 100;

  free(ports);
}

void JackMidi::activate()
{
  std::cout << "activate()" << std::flush;

  if (jack_activate(client) != 0)
  {
    std::cout<<  "cannot activate client" << std::endl;
    return;
  }

  // connect midi port one
  if(jack_connect(client,portsOut[2],jack_port_name(inputPort))){
    std::cout << "Cannot connect output ports\n";
  }
  // connect midi port two
  if(jack_connect(client,portsOut[3],jack_port_name(inputPort))){
    std::cout << "Cannot connect output ports\n";
  }


  std::cout << "\t\tDone!" << std::endl;
}

int JackMidi::staticProcess(jack_nframes_t nframes, void *arg)
{
  return static_cast<JackMidi*>(arg)->process(nframes);
}


//get midi data-----------------------------------------------------------------
int JackMidi::process(jack_nframes_t nframes)
{
  jack_midi_event_t       in_event;
  jack_position_t         position;
  jack_transport_state_t  transport;

  // get the port data
  void* port_buf = jack_port_get_buffer( inputPort, nframes);

  // get the transport state of the JACK server
  transport = jack_transport_query( client, &position );

  // input: get number of events, and process them.
  jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
  if(event_count > 0)
  {
    for(int i=0; i<event_count; i++)
    {
      jack_midi_event_get(&in_event, port_buf, i);

      /* print pots array
      for(int i = 0; i < POTAMNT; i++){
        std::cout << pots[i] << " ";
      }
      std::cout << "\n";
      */

      //if is midi cc data
      //fill pots array with midi data
      if((long)in_event.buffer[0] == 176){
        int id = (long)in_event.buffer[1] - 1;
        int param = (long)in_event.buffer[2];

        pots[id] = param;
        //scaling of midi data
        if(id == 0){ //coarse1
          calculateCoarse(param, id);
          for(int i = 0; i < VOICEAMNT; i++){
            complexFreqArray[i] = mtof(noteArray[i]) * scaledParams[0];
          }
        }else
        if(id == 1){ //fm1
          calculateFM(param, id);
          fm = scaledParams[1];
        }else
        if(id == 2){ //level1
          calculateLowPassGate(param, id);
          volume1 = (float)param / (float)127;
          lowPassFreq1 = scaledParams[2];
        }else
        if(id == 3){ //shape1
          calulateWaveShaperShape(param, id);
          shape1 = scaledParams[3];
        }else
        if(id == 4){ //square1
          calulateWaveShaperSquare(param, id);
          square1 = scaledParams[4];
        }else
        if(id == 5){ //timbre
          calulateWaveShaperTimbre(param, id);
          timbre = scaledParams[5];
        } else
        if(id == 6){ //coarse2
          calculateCoarse(param, id);
          for(int i = 0; i < VOICEAMNT; i++){
            oscillator2FreqArray[i] = mtof(noteArray[i]) * scaledParams[6] * scaledParams[7];
          }
        }else
        if(id == 7){ //fine
          calculateFine(param, id);
          for(int i = 0; i < VOICEAMNT; i++){
            oscillator2FreqArray[i] = mtof(noteArray[i]) * scaledParams[6] * scaledParams[7];
          }
        }else
        if(id == 8){ //level2
          calculateLowPassGate(param, id);
          volume2 = (float)param / (float)127;
          lowPassFreq1 = scaledParams[8];
        }else
        if(id == 9){ //shape2
          calulateWaveShaperShape(param, id);
          shape2 = scaledParams[9];
        }else
        if(id == 10){ //square2
          calulateWaveShaperSquare(param, id);
          square2 = scaledParams[10];
        }else
        if(id == 11){ //ringMod
          calulateRingmod(param, id);
          ringMod = scaledParams[11];
        }else
        if(id == 12){ //attack
          calulateAttack(param, id);
          attack = scaledParams[12];
        }else
        if(id == 13){ //decay
          calulateDecay(param, id);
          decay = scaledParams[13];
        }else
        if(id == 14){ //sustain
          calulateSustain(param, id);
          sustain = scaledParams[14];
        }else
        if(id == 15){ //release
          calulateRelease(param, id);
          release = scaledParams[15];
        }

      }
      //note on
      if((long)in_event.buffer[0] == 144){
        midNote = (long)in_event.buffer[1];
        //calculate new frequency
        //note == 0 is note of
        if(midNote == 0){
          midVel = 0;
          poly(midNote,midVel);
        }

        midVel = 100;
        poly(midNote,midVel);

        for(int i = 0; i < VOICEAMNT; i++){
          complexFreqArray[i] = mtof(noteArray[i]) * scaledParams[0];
        }
        for(int i = 0; i < VOICEAMNT; i++){
          oscillator2FreqArray[i] = mtof(noteArray[i]) * scaledParams[6] * scaledParams[7];
        }

      }
      //note off
      if((long)in_event.buffer[0] == 128){
        midVel = 0;
        midNote = (long)in_event.buffer[1];
        poly(midNote,midVel);
      }

    }
  }

  return 0;
}

//Calculate values--------------------------------------------------------------
//function for midi to frequency
float JackMidi::mtof(int midiIn){
  float frequency;

  frequency = pow(2,((float)midiIn-69)/(float)12) * (float)440;//440hz as base fq

  return frequency;
}

void JackMidi::calculateCoarse(int coarse,int identifier){
  int   octaveSwitch = coarse/30;
  float octave = 1;

  if(octaveSwitch == 0){
    octave = 0.25;
  }else
  if(octaveSwitch == 1){
    octave = 0.5;
  }else
  if(octaveSwitch == 2){
    octave = (float)1;
  }else
  if(octaveSwitch == 3){
    octave = (float)2;
  }else
  if(octaveSwitch == 4){
    octave = (float)4;
  }

  scaledParams[identifier] = octave;
}

void JackMidi::calculateFine(int fine,int identifier){

  scaledParams[identifier] = (float)fine / (float)127 + (float)1;

}

void JackMidi::calculateFM(int fm, int identifier){
  scaledParams[identifier] = (float)fm / 12.7;

}

void JackMidi::calculateLowPassGate(int volume ,int identifier){
  float lowPassFreq;
  float amp;

  amp = (float)volume / (float)127;

  lowPassFreq =  amp * amp;//pow2
  lowPassFreq = (amp * 2300.0) + 40.0; //scaling
  lowPassFreq = sin(lowPassFreq * TWOPI / (float)SAMPLERATE); //curcular frequenct

  scaledParams[identifier] = lowPassFreq;
}

void JackMidi::calulateWaveShaperShape(int shape ,int identifier){
  scaledParams[identifier] = ((float)shape / (float)127 * (float)9) + (float)1;

}

void JackMidi::calulateWaveShaperSquare(int square ,int identifier){
  scaledParams[identifier] = ((float)square / (float)127 * (float)5) + (float)1;

}

void JackMidi::calulateWaveShaperTimbre(int timbre ,int identifier){
  float waveShaper;

  waveShaper = (float)timbre  / (float)127 * 0.99;
  waveShaper = waveShaper * waveShaper * waveShaper; //make exponential

  scaledParams[identifier] = waveShaper;

}

void JackMidi::calulateRingmod(int ringmod ,int identifier){
  scaledParams[identifier] = (float)ringmod / (float)127;

}

void JackMidi::calulateAttack(int attack ,int identifier){
  scaledParams[identifier] = attack * 20;

}

void JackMidi::calulateDecay(int decay ,int identifier){
  scaledParams[identifier] = decay * 20;

}

void JackMidi::calulateSustain(int sustain ,int identifier){
  scaledParams[identifier] = (float)sustain / (float)127;

}

void JackMidi::calulateRelease(int release ,int identifier){
  scaledParams[identifier] = release * 20;

}

//getting scaled values back to main
void JackMidi::getScaledValues
(
  float *complexOscFreq, float &fm, float &volume1, float &lowPassFreq1,
  float &shape1, float &square1, float &timbre,
  float *oscillator2Freq, float &volume2, float &lowPassFreq2,
  float &shape2, float &square2,
  float &ringMod,
  int *velocity, int &attack, int &decay, float &sustain, int &release
){

  fm = this->fm;
  volume1 = this->volume1;
  lowPassFreq1 = this->lowPassFreq1;
  shape1 = this->shape1;
  square1 = this->square1;
  timbre = this->timbre;
  volume2 = this->volume2;
  lowPassFreq2 = this->lowPassFreq2;
  shape2 = this->shape2;
  square2 = this->square2;
  ringMod = this->ringMod;
  attack = this->attack;
  decay = this->decay;
  sustain = this->sustain;
  release = this->release;

  for(int i = 0; i < VOICEAMNT; i++){
    complexOscFreq[i] = complexFreqArray[i];
    oscillator2Freq[i] = oscillator2FreqArray[i];
    velocity[i] = velocityArray[i];
  }//for

}

void JackMidi::getArrays(int *velocity){


}


//function for polyphony
//this function is without voice steal
void JackMidi::poly(int midiIn, int velocity){
  for(int i = 0; i < VOICEAMNT; i++){
    if(velocity != 0){//if note on
      if(velocityArray[i] == 0){//search for note off
        noteArray[i] = midiIn;
        velocityArray[i] = 100;
        break;
      }//if
    } //if note off search for note that is one to turn off
    if(noteArray[i] == midiIn){
      velocityArray[i] = 0;
      break;
    }//if
  }//for

}//poly

JackMidi::~JackMidi(){
}
