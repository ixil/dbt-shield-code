// http://www.gammon.com.au/interrupts
// http://www.gammon.com.au/timers


volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;
float lastTempReading;
float lastTempReading;

#include "TemperatureSensing.h"


void setup ()
{
  Serial.begin (115200);
  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
  ADMUX = bit (REFS0) | (adcPin & 0x07);

}  // end of setup

// ADC complete ISR
ISR (ADC_vect)
  {
  byte low, high;
  
  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  low = ADCL;
  high = ADCH;

  adcReading = (high << 8) | low;
  adcDone = true;  
  }  // end of ADC_vect

void loop ()
{
  readbyte
  // if last reading finished, process it
  if (adcDone)
    {
    adcStarted = false;

    // do something with the reading, for example, print it
    Serial.println (adcReading);
    delay (500);

    adcDone = false;
    }
    
  
  // do other stuff here

}  // end of loop


ISR (PCINT0_vect)
 {
 // handle pin change interrupt for D8 to D13 here
 }  // end of PCINT0_vect

ISR (PCINT1_vect)
 {
 // handle pin change interrupt for A0 to A5 here
 }  // end of PCINT1_vect

ISR (PCINT2_vect)
 {
 // handle pin change interrupt for D0 to D7 here
 }  // end of PCINT2_vect


void setup ()
  { 
  // pin change interrupt (example for D9)
  PCMSK0 |= bit (PCINT1);  // want pin 9
  PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
  PCICR  |= bit (PCIE0);   // enable pin change interrupts for D8 to D13
  }

// | attachInterrupt | Name | PinOnChip | Pin on Board |
// | 0               | INT4 | 6         | D2
// | 1               | INT5 | 7         | D3
// | 2               | INT0 | 43        | D21
// | 3               | INT1 | 44        | D20
// | 4               | INT2 | 45        | D19
// | 5               | INT3 | 46        | D18

// D0	  PCINT16 (PCMSK2 / PCIF2 / PCIE2)
// D1	  PCINT17 (PCMSK2 / PCIF2 / PCIE2)
// D2	  PCINT18 (PCMSK2 / PCIF2 / PCIE2)
// D3	  PCINT19 (PCMSK2 / PCIF2 / PCIE2)
// D4	  PCINT20 (PCMSK2 / PCIF2 / PCIE2)
// D5	  PCINT21 (PCMSK2 / PCIF2 / PCIE2)
// D6	  PCINT22 (PCMSK2 / PCIF2 / PCIE2)
// D7	  PCINT23 (PCMSK2 / PCIF2 / PCIE2)
// D8	  PCINT0  (PCMSK0 / PCIF0 / PCIE0)
// D9	  PCINT1  (PCMSK0 / PCIF0 / PCIE0)
// D10	  PCINT2  (PCMSK0 / PCIF0 / PCIE0)
// D11	  PCINT3  (PCMSK0 / PCIF0 / PCIE0)
// D12	  PCINT4  (PCMSK0 / PCIF0 / PCIE0)
// D13	  PCINT5  (PCMSK0 / PCIF0 / PCIE0)
// A0	  PCINT8  (PCMSK1 / PCIF1 / PCIE1)
// A1	  PCINT9  (PCMSK1 / PCIF1 / PCIE1)
// A2	  PCINT10 (PCMSK1 / PCIF1 / PCIE1)
// A3	  PCINT11 (PCMSK1 / PCIF1 / PCIE1)
// A4	  PCINT12 (PCMSK1 / PCIF1 / PCIE1)
// A5	  PCINT13 (PCMSK1 / PCIF1 / PCIE1)
