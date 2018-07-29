/**********************************************************************************
  
  KIKGEN MIDI USB 
  A MIDI USB RELIABLE INTERFACE

  USB-MIDI simple converter Sketch for Arduino


       USB                           ATMEGA8U2                    ATMEGA 328P
  --------------         ------------------------------         ---------------
  IN Endpoint  o<--------o USBOUT | usbMidiKliK |  RX o<--------o (TX) MIDI IN
  OUT Endpoint o-------->o USBIN  |  firmware   |  TX o-------->o (RX) MIDI OUT   
  
  The RX/TX on the Arduino must no be crossed, as the ATMEGA8U2 is directly connected
  to the RX and TX pins.  When PIN0 (RX) and PIN1 (TX) are configured as INPUT, we
  can talk directly with the ATMEGA8U2 managing the USB, making the Arduino transparent.
  
************************************************************************************/

void resetMidiUSB() {

       Serial.begin(31250);

      // Send SYSEX Message # 0A
      Serial.write( 0xF0 );
      Serial.write( 0x77 );
      Serial.write( 0x77 );
      Serial.write( 0x77 );
      Serial.write( 0x0A );
      Serial.write( 0xF7 );      

}

void setVendorProductIds(uint16_t vendorID,uint16_t productID) {

      Serial.begin(31250);

      // Send SYSEX Message # 0C
      Serial.write( 0xF0 );
      Serial.write( 0x77 );
      Serial.write( 0x77 );
      Serial.write( 0x77 );
      Serial.write( 0x0C );

      Serial.write( (vendorID & 0xF000)  >> 12 );
      Serial.write( (vendorID & 0x0F00)  >> 8  );
      Serial.write( (vendorID & 0x00F0)  >> 4  );
      Serial.write(  vendorID & 0x000F );

      Serial.write( (productID & 0xF000)  >> 12 );
      Serial.write( (productID & 0x0F00)  >> 8  );
      Serial.write( (productID & 0x00F0)  >> 4  );
      Serial.write(  productID & 0x000F );

      Serial.write( 0xF7 );        
      resetMidiUSB();
}

void SetProductString() {

      Serial.begin(31250);
      // Send SYSEX Message # 0B
      Serial.write( 0xF0 );
      Serial.write( 0x77 );
      Serial.write( 0x77 );
      Serial.write( 0x77 );
      Serial.write( 0x0B );
      
      // Important : do not use accentuated characters to avoid 8 bits values
      char * ProductString = "USB MIDI Demo";

      for (uint8_t i=0; *(ProductString + i ) !=0 ; i++ ) {
         Serial.write(* ( ProductString + i ) );
      }
      
      Serial.write( 0xF7 ); 
      resetMidiUSB();       
}

// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

void setup() {

  // Make Arduino transparent for serial communications from and to USB
  pinMode(0,INPUT); // Arduino RX - ATMEGA8U2 TX
  pinMode(1,INPUT); // Arduino TX - ATMEGA8U2 RX
  pinMode(A0, INPUT_PULLUP); // Pull up resistor for ribbon.

  resetMidiUSB();
  setVendorProductIds( 1, 1);
  SetProductString();

}

const int NOTE_ON_CH0 = 0x90;

const int MAXRES = 678;
const int MINRES = 13;

const int MAXNOTE1 = 83;
const int MINNOTE1 = 60;

const int MAXNOTE2 = 90;
const int MINNOTE2 = 67;

const int MAXNOTE3 = 97;
const int MAXNOTE3 = 74;

int transpose = 0;

bool playing = false;
int current_note = 0;

int getCurrentNote( int ribbonNumber, int aVal ) {
  int maxNote = 0;
  int minNote = 0;
  switch( ribbonNumber )
  {
    case 1:
      maxNote = MAXNOTE1;
      minNote = MINNOTE1;
      break;
    case 2:
      maxNote = MAXNOTE2;
      minNote = MINNOTE2;
      break;
    case 3:
      maxNote = MAXNOTE3;
      minNote = MINNOTE3;
      break;
    default:
      return 0;      
  }
  return map( aVal, MINRES, MAXRES, minNote + transpose, maxNote + transpose );
}


void loop() {


  int pot1_in = analogRead(A0);

  if ( po1t_in > 800 ) {
    // The string in not being pressed so stop a note if it is playing
    if ( playing ) {
      noteOn( NOTE_ON_CH0, current_note, 0x00 );
        playing = false;
    }
  } else {
    int new_note = getCurrentNote( pot1_in );
    // A note is pressed. If one is playing, stop it unless it is the same note
    if ( current_note != new_note ) {

      if ( playing ) {
        noteOn( NOTE_ON_CH0, current_note, 0x00 );
      }
      current_note = new_note;
      playing = true;
      noteOn( NOTE_ON_CH0, current_note, 0x45 );
    }
  }


  delay(1);


}


