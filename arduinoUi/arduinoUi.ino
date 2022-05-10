#define potAmnt 16

// define variables for the controller data
byte analogValue[potAmnt];  
byte lastAnalogValue[potAmnt];
byte midiCCselect[potAmnt]; // select the midi Controller Number for each input
int thresh[potAmnt]; // select threshold for each analog input  


void setup() {
    //Serial.begin(9600);
   Serial1.begin(31250); //  Set MIDI baud rate:

  //initialise arrays
  for(int i = 0; i < potAmnt; i++){
    analogValue[i] = 0;
    lastAnalogValue[i] = 0;
    midiCCselect[i] = i+1;
    thresh[i] = -1;
  }
   
}

void loop() {
  
  for(int i = 0; i < potAmnt; i++){
    analogValue[i] = analogRead(i)/8; //scale down to single byte
    if(analogValue[i] > thresh[i]) {
      if(analogValue[i] != lastAnalogValue[i]) {
        midiCC(0xB0, midiCCselect[i], analogValue[i]);
        lastAnalogValue[i] = analogValue[i];

      }//if
    }//if   
  }//for

  delay(1);
}

//function for sending Midi CC. 
void midiCC(char CC_data, char c_num, char c_val){
   Serial1.write((byte)CC_data);
   Serial1.write((byte)c_num);
   Serial1.write((byte)c_val);

}
