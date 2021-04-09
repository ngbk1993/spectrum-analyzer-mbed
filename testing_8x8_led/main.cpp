/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "stats_report.h"

DigitalOut latchPinCol(D8);
DigitalOut latchPinRow(D7);
DigitalOut clockPin(D12);
DigitalOut dataPin(D11);

char your_text[7] = {
	0b00000000,
	0b01101100,
	0b10010010,
	0b10000010,
	0b01000100,
	0b00101000,
        0b00010000,
};

#define SLEEP_TIME                  500 // (msec)
#define PRINT_AFTER_N_LOOPS         20
Serial pc(USBTX, USBRX);

void shiftOut1(DigitalOut& mylatchPin, DigitalOut& myDataPin, DigitalOut&  myClockPin, char myDataOut) {

  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low
 //internal function setup
  int i = 0;
  int pinState;

 //clear everything out just in case to
 //prepare shift register for bit shifting
  mylatchPin = 0;
  myDataPin  = 0;
  myClockPin = 0;
  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
  //for (i=0; i<8; i++)  {
    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    myClockPin = 0;
    if ( myDataOut &  (1<<i) ) {
      
      pinState = 1;

    } else { 

      pinState = 0;

    }
    //Sets the pin to HIGH or LOW depending on pinState
    myDataPin = pinState;
    //register shifts bits on upstroke of clock pin  i
    myClockPin = 1;
    //zero the data pin after shift to prevent bleed through
    myDataPin  = 0;
  }
  myClockPin = 0;
  mylatchPin = 1;
}
// main() runs in its own thread in the OS
int main()
{
    SystemReport sys_state( SLEEP_TIME * PRINT_AFTER_N_LOOPS /* Loop delay time in ms */);

  for (int i=0 ; i < 8 ; i++)
  { 
    shiftOut1(latchPinCol, dataPin, clockPin, 1); 
  }



    while (true) {
        // Blink LED and wait 0.5 seconds
        pc.printf("Blink! LED is now");
        sys_state.report_state();
      for (int kk=0; kk <50; kk++){
          for (int j = 0; j < 8; j++) {
          //ground latchPin and hold low for as long as you are transmitting
          shiftOut1(latchPinRow, dataPin, clockPin, 1  << j);
    	  shiftOut1(latchPinCol, dataPin, clockPin, 0xff - your_text[j]);
	  wait_ms(1);
          }  
	  

       }
  }
}
