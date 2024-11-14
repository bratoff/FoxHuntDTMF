//  FoxHuntDTMF - Fox with DTMF control for ATTiny85
//  TRANSMITTER LOGIC FOR HT-based foxhunt fox
//  UPLOAD USING ATtiny85, 8MHz Internal NO BOD Core!!!!  (DigiSpark core is fine.)
//  Aug 2021 - Accepts remote commands via DTMF - see below
//  -------------------------------------------------------------
// Modification history:
//  - Replace Arduino bitwise functions with macros for smaller code
//  - Replace numeric literal constants with #defines for better readability
//  - Compile-time option replaces music with a series of beeps (Nov 2024)
//

/*
 *  Instructions:
  *  Lost/found switch:
  *   1. Open or high = Lost, closed or low = Found.
  *   2. If power is applied while "Found", hunt starts when switch is moved to "Lost"
  *   3. If power is applied while "Lost", hunt starts when DTMF Enable command is sent.
  *   4. Switch is dynamic - it only changes the hunt state when thrown - DTMF can still override it
  *      i.e., the state of the fox will be whichever occurred last, the switch change or the DTMF command.  
  *      The '5' command will always send back the fox's current state so you can be sure.
  *  DTMF commands:
  *   0 - disable transmission of Fox messages (just listen for further commands)
  *       Fox responds with "QRT"
  *   1 - enable Fox transmissions.  Current message (Lost or Found) will be sent immediately.
  *   4 - put fox in "found" state - sends back "F" - overrides the switch
  *   5 - status - sends back "L" for lost or "F" for found
  *   6 - put fox in "lost" state and enable transmission (like 7 followed by 1)
  *   7 - put fox in "lost" state - sends back "L" - overrides the switch
  *   9 - Send Fox Lost/Found message right now if transmission is enabled
  *   A - Read Vcc - sends back "BV xx" - xx is tenths of a volt
  *   B - blink LED continuously when not transmitting
  *   D - toggles Tx LED on and off with PTT - sends back "ON" or "OF"
  *   Any other key will be echoed back in morse, followed by '?'
  *   
  *   NOTE:  0 and 1 only affect the "lost" and "found" messages.  The command responses are always enabled.
 */

//#define MUSIC

#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <tinyDTMF.h>

#ifdef MUSIC
#include "pitches.h"
#include "music.h"
#endif

#include "cwtable.h"

// Port manipulation macros save space by avoiding digitalRead and digitalWrite
#define SetBit(p,b) (p |= (1<<b))
#define ClearBit(p,b) (p &= ~(1<<b))
#define ReadBit(p,b) ((p & (1<<b)) != 0)

// Pin assignments - leave TXLed undefined to disable TX light
#define TXLed LED_BUILTIN   // Define Led TX Indicator Pin
#define tonePin 0           // Define Audio out Pin
#define foundPin 2          // Ground this pin when fox is found
#define PTT 4               // Define PTT Pin 
#define dtmfPin A0          // Audio input pin for DTMF (A0 is PB5)

#define toneFreq 800        // Define Morse Tone Frequency in Hz
#define cspeed 2
#define mspeed 1
#define wholeNote 800
#define transpose 1
#define huntPause 180000UL
#define foundPause 30000UL
#define lostMsgTime 10000UL

#define VREF 0x0c           // Pseudo-pin number for internal 1.1v reference
#define MIN_BATT 40         // Minimum Vcc allowed in tenths of a volt
#define REF_CONST 1125300UL   // Constant for voltage calculation = 1.1*1023*1000

//Enter your callsign here
const char callSign[] PROGMEM = "KO4XL/F";

//Morse Strings to be Sent - use dash to get extra time between characters
const char morseLost[] PROGMEM = "LOST";
const char morseFound[] PROGMEM = "FOUND";
const char QRT[] PROGMEM = "QRT";
const char QRV[] PROGMEM = "QRV";
const char lowBatt[] PROGMEM = "LO BAT";

unsigned long pauseTime;  // Holds current long wait time (either huntPause or foundPause)

uint8_t huntState;           // 1 = hunt active, 0 = fox found
#define HUNT_ACTIVE 1
#define FOX_FOUND 0

uint8_t switchState;         // Last state of lost/found switch (1=lost, 0=found)
#define SWITCH_LOST 1
#define SWITCH_FOUND 0

uint8_t foxEnabled;          // bit 0 set if fox is enabled to cycle, bit 1 set to tx once
#define FOX_ENABLED 1
#define FOX_TX_ONCE 2
#define FOX_DISABLED 0

#ifdef TXLed
char ledEnabled = 0;      // nonzero if TX led is enabled
char blinker = 0;         // nonzero to blink Tx LED while idle
#endif

tinyDTMF DTMF(A0);        // Declare tone decoder
char dtmfKey = 0;         // Last DTMF key received
char waitForSpace = 0;    // Nonzero if waiting for DTMF to stop

void setup(){
    DTMF.begin();
#ifdef TXLed
    DDRB |= ((1<<TXLed) | (1<<tonePin) | (1<<PTT));
#else
    DDRB |= ((1<<tonePin) | (1<<PTT));
#endif
    ClearBit(DDRB,foundPin);
    SetBit(PORTB,foundPin);
    pauseTime = 1000UL;
    foxEnabled = FOX_DISABLED;
    if(!ReadBit(PINB,foundPin)) {       // If switch was in "found" position at power up
      switchState = SWITCH_FOUND;
      while(!ReadBit(PINB,foundPin)) {     // Wait until switch is in "lost" position
        sdelay(pauseTime);            // use long delay so that we still watch the DTMF
      }
    }
    foxEnabled = FOX_ENABLED;                 // Fox is enabled when switch goes to "lost"
    pauseTime = huntPause;
    switchState = SWITCH_LOST;
    huntState = HUNT_ACTIVE;
}

// Returns true when a DTMF key has been pressed and released
bool checkDTMF(void) {
    char key = 0;
    bool retVal = false;
    
    DTMF.getSample();
    key = DTMF.procSample();
    if(waitForSpace) {
      if(key == 0) {
        waitForSpace = 0;
        if(dtmfKey != 0) {
          retVal = true;
        }
      }
    } else {
      if(key != 0) {
        dtmfKey = key;
        waitForSpace = 1;
      }
    }
    return(retVal);
}

// Key the radio
void pttDown(void) {
#ifdef TXLed
    if(ledEnabled) 
      SetBit(PORTB, TXLed);
#endif
    delay(1000);               //Always wait 1 sec first so commands can unkey
    SetBit(PORTB, PTT);
    sdelay(1000);               //Wait for Radio to TX
}

// Unkey the radio
void pttUp(void) {
    sdelay(500);               //Wait for last element before unkeying
    ClearBit(PORTB, PTT);
#ifdef TXLed
    if(ledEnabled) 
      ClearBit(PORTB, TXLed);
#endif
}

//Send a cw string from PROGMEM
void SendMorse(const char message[]) {
  int i;
  char j;

  for(i=0; (j=pgm_read_byte(&message[i])); i++) {
    if(j > 'Z')
      j -= ' ';
    sendChar(j);
  }
}

//DAH Function - send a DAH
//void dah(int loops){
void dah(void) {
      tone(tonePin,toneFreq);
      sdelay(385/cspeed);
      noTone(tonePin);
      sdelay(90/cspeed);
}

//DIT Function - send a DIT
//void dit(int loops){
void dit(void){
      tone(tonePin,toneFreq);
      sdelay(125/cspeed);
      noTone(tonePin);
      sdelay(100/cspeed);
}

//WordBreak Function - delay between words
void wordBreak(void){
    noTone(tonePin);
    sdelay(850/cspeed);
}

//CharBreak Function - delay between cw characters
void CharBreak(void){
    noTone(tonePin);
    sdelay(165/cspeed);
}

void sendElements(unsigned char ele) {
  unsigned char bits = ele;
  char i,j;

  for(i=0; i<4; i++) {
    j = (bits & 0xC0) >> 6;
    bits <<= 2;
    switch(j) {
      case DIT:
        dit();
        break;
      case DAH:
        dah();
        break;
      case WB:
        wordBreak();
        break;
      default:
        break;
    }
  if(!bits)
    break;
  }
}

// Send one cw character
void sendChar(const char c) {
  unsigned char k,m;
  
  for(k=0; (m=pgm_read_byte(&chtable[k])); k++) {
    if(c == m)
      break;
  }
  if(m == '-') {
    CharBreak();
  } else if(m) {
    k <<= 1;
    sendElements(pgm_read_byte(&cwtable[k]));
    sendElements(pgm_read_byte(&cwtable[k+1]));
  }
  if((c != ' ') && (c != '-'))
    CharBreak();
}

// Return measured Vcc in tenths of a volt
int readVcc(void) {
  ADMUX = VREF;
  delay(2);               // switching mux requires settling time
  ADCSRA |= (1<<ADSC);    // Start conversion
  while(ADCSRA & (1<<ADSC));  // Wait for conversion to complete
  uint8_t low = ADCL;     // Get raw result
  uint8_t high = ADCH;
  unsigned long value = (high << 8) | low;  // combine for 10 bit result
  value = REF_CONST / value;  // compute Vcc in millivolts
  value += 50;            // round up
  value /= 100;           // convert to tenths of volts
  return value;
}

// Read and send measured Vcc
void sendVcc(void) {
  int v = readVcc();
  sendChar('B');
  sendChar('V');
  sendChar(v/10+'0');
  sendChar(v%10+'0');
}

// status - send L or F for lost or found
void sendLF() {
  pttDown();
  if(huntState == HUNT_ACTIVE)
    sendChar('L');
  else
    sendChar('F');
  pttUp();
}

void setLost() {
  huntState = HUNT_ACTIVE;               //   set hunt state to lost
  pauseTime = huntPause;
  sendLF();
}

// Take action if a DTMF key has been captured
// Return true if command should break out of long delay
bool processKey(void) {
  bool txNow = false;
  if(dtmfKey != 0) {        // if a DTMF key was captured
    switch(dtmfKey) {       // Choose function
      case '0':
        foxEnabled = FOX_DISABLED;     // Disable fox from repeating message
        pttDown();
        SendMorse(QRT);     // Acknowledge the disable
        pttUp();
        break;
      case '1':
        foxEnabled = FOX_ENABLED;     // Enable repeating transmission
        txNow = true;
        break;
      case '4':
        huntState = FOX_FOUND;              // set hunt state to found
        pauseTime = foundPause;
        txNow = foxEnabled;         // If we're enabled, start transmission now
        sendLF();                   // and send status
        break;
      case '5':
        sendLF();                    // send current status
        break;
      case '6':
        foxEnabled = FOX_ENABLED;             // hunt mode and enable in one command
        setLost();
        break;
      case '7':
        setLost();
        break;
      case '9':
        txNow = true;          // Send lost/found message once - retains enabled/disabled state
        foxEnabled |= FOX_TX_ONCE;
        break;
      case 'A':
        pttDown();            // Measure and send Vcc
        sendVcc();
        pttUp();
        break;
#ifdef TXLed
      case 'D':
        ledEnabled = !ledEnabled;
        pttDown();
        sendChar('O');
        if(ledEnabled)
          sendChar('N');
        else
          sendChar('F');
        pttUp();
        break;
      case 'B':
        blinker = !blinker;
        ClearBit(PORTB, TXLed);
        break;
#endif
      default:
        pttDown();
        sendChar(dtmfKey); sendChar('?');
        pttUp();
    }
  }
  dtmfKey = 0;
  return(txNow);
}

// Wait ms milliseconds while watching Lost/Found switch
// If this is the long wait, also watch for DTMF commands
void sdelay(unsigned long ms) {
  unsigned long millis_then = millis() + ms;
  unsigned long millis_dtmf = millis();
  unsigned long millis_blink = millis();
  char foundWas = SWITCH_LOST;    // Last state of switch during debounce
  char debounce = 0;    // Count since switch last changed
  char t;

  while((long)(millis()-millis_then) < 0) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_mode();

    // DTMF and found switch processing only happen in the idle between transmissions
    // This is done because the other delays are critical to music or cw timing
    if(ms == pauseTime) {
      if((long)(millis()-millis_dtmf) > 0) { // To save battery, only check DTMF every 100 ms
        if(checkDTMF()) {
          if(processKey()) {                  // If we got a DTMF key process it
            break;                           // Terminate delay if DTMF command requires it
          }
        }
        millis_dtmf = millis() + 100;     // Don't check DTMF for another 100 ms
      }
      t = ReadBit(PINB,foundPin);  // Next read the found switch and debounce it
      if(foundWas == t) {
        ++debounce;               // If it hasn't changed, increment counter
      } else {
        debounce = 0;             // Otherwise, capture new value and reset counter
        foundWas = t;
      }
      if(debounce > 100) {        // If switch stayed stable for full debounce count
        if(switchState != foundWas) {   // If switch was actually changed by a user
          switchState = foundWas;
          if((foundWas == 1) && (huntState == FOX_FOUND)) {     // If switch says lost and huntState says found
            huntState = HUNT_ACTIVE;                              //   set hunt state to lost
            pauseTime = huntPause;
            break;
          }
          if((foundWas == 0) && (huntState == HUNT_ACTIVE)) {     // If switch says found and huntState says lost
            huntState = FOX_FOUND;                              //   set hunt state to found
            pauseTime = foundPause;
            break;
          }
        }
      }
#ifdef TXLed
      if(blinker && (long)(millis()-millis_blink) > 0) {
        SetBit(PINB, TXLed);
        millis_blink = millis() + 250;
      }
#endif
    }  // End if(ms == pauseTime)
  } // End while((long)(millis()-millis_then) < 0)
} // End sdelay();

void loop(){
  if(foxEnabled != FOX_DISABLED) {      // Any non-silent mode
    foxEnabled &= FOX_ENABLED;    // Keep only the enabled flag, not any temporaries
    pttDown();
    if(huntState == HUNT_ACTIVE) {
      SendMorse(morseLost);
#ifdef MUSIC
      sdelay(1000);           //Wait a sec before playing melody(s)
      playMelody(notes_Twinkle, melody_Twinkle, durations_Twinkle);           //Play Melody #1
#endif
      sdelay(1000);           //Wait between melodies
      if(readVcc() > MIN_BATT) {
#ifdef MUSIC
        playMelody(notes_Zelda, melody_Zelda, durations_Zelda);           //Play Melody #2
#else
        unsigned long stopMsgTime = millis() + lostMsgTime;
        do {
          dah();
          sdelay(500);
        } while((long)(millis()-stopMsgTime) < 0);
#endif
      } else {
        SendMorse(lowBatt);     // if battery running low, replace melody #2 with warning
        sdelay(1000);
      }
    } else {
      SendMorse(morseFound);
#ifdef MUSIC
      sdelay(1000);
      playMelody(notes_StarWars, melody_StarWars, durations_StarWars);
#endif
    }
    sdelay(1000);               //Wait 
    if(huntState == HUNT_ACTIVE)
      SendMorse(morseLost);
    else
      SendMorse(morseFound);
    wordBreak();
    SendMorse(callSign);        // Always follow message by callsign
    pttUp();
  }
  sdelay(pauseTime);          //Wait to start over - meanwhile listen for commands
}

#ifdef MUSIC
// Play a melody, given note count, list of pitches and list of durations
void playMelody(const int numberOfNotes, const int melody[], const char noteDurations[]) {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < numberOfNotes; thisNote++) {
        // to calculate the note duration, take one second
        // divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = wholeNote/pgm_read_byte(&noteDurations[thisNote])/mspeed;
        if(noteDuration < 0) {
          noteDuration = -noteDuration;     // Negative number means dotted note
          noteDuration += noteDuration/2;
        }
        int pitch = pgm_read_word(&melody[thisNote]);
        tone(tonePin, pitch*transpose, noteDuration);
    
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration + noteDuration/3;
        sdelay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(tonePin);
  }  
}
#endif
