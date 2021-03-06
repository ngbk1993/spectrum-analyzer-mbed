#include <arm_math.h>
#include "mbed.h"
#include "stats_report.h"
#include <arm_const_structs.h>
// Uses Daniil Guitelson's BGI library


DigitalOut latchPinCol(D8);
DigitalOut latchPinRow(D7);
DigitalOut clockPin(D12);
DigitalOut dataPin(D11);


Serial pc(USBTX, USBRX);

// 16 point FFT
#define N		32
float   sample[2*N];
float   magnitudes[N];
uint8_t led_buf[8];
const static arm_cfft_instance_f32 *S;
// End of FFT declaration

//AnalogDeclaration
volatile int kbhit;
AnalogIn MicIn(A0);

void vUpdate(void){
    kbhit = 1;
}

void read_adc_to_memory()
{
    Ticker Period;
    kbhit = 0;   
    Period.attach_us(&vUpdate,25);
    //int harvest=0;
    for (int i = 0; i < N*2 ; i+=2)
    //for (int i=0; i < N*2 ; i++)
    {  
        while(kbhit==0);
        kbhit=0;
	//harvest = MicIn.read_u16();
        sample[i] = MicIn.read();//buffer[i];
        sample[i+1] = 0;
    }
    
    Period.detach();
}

void shiftOut(DigitalOut& mylatchPin, DigitalOut& myDataPin, DigitalOut&  myClockPin, char myDataOut) {

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
  //for each bit in the byte myDataOutÃ¯Â¿Â½
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

void update_8x8_led(){

int SCAN_FREQUENCY = 25;

    for (int scanning_freq = 0; scanning_freq < SCAN_FREQUENCY; scanning_freq++){
        for (int j = 0; j < 8; j++) {
            //ground latchPin and hold low for as long as you are transmitting
            shiftOut(latchPinRow, dataPin, clockPin, 1  << j);
    	    shiftOut(latchPinCol, dataPin, clockPin, 0xff-led_buf[j]);
	    wait_us(100);
        }  
    }
}


void mag_to_buf()
{
    
    float xmax = 0;
    int   temp = 0;
    float temp_float = 0;
    float logged[N];
    int led = 0;
 //  for (int i = 0; i < N; i++)
 //  {		
       //logged[i] = (float)log( 30.0 * (double) magnitudes[i] + 1 );
 //  }
   
   for (int i = 1; i < N; i++)
   {
       if (magnitudes[i] > xmax)
       {
           xmax = magnitudes[i];
       }
   }
   
   if (magnitudes[0] - xmax > 1.0) 
   {
   
      //scaled data
       for (int i = 1 ; i < 9  ; i++)
       {   
            temp_float = magnitudes[i];
	    //if (temp_float > xmax)
	    //{
	    //    temp_float = xmax;
	    //}
            //temp = (int)pow(2,((int)magnitudes[i+2]*(N/2)/(int)xmax))-1;
	    //printf("%f\n",temp_float);
	    led = (int)(temp_float * 8.0 / (magnitudes[0]*70/100));
	    //printf("%d",led);
            if (led > 8){led = 8;}

	    temp = (int)pow(2,(double)led)-1;
	    led_buf[i-1] = temp; 
        }

    } else {
       for (int i = 0; i < 8; i++)
       {
	   led_buf[i] = 0;
       }
    }
}

int main(void)
{
    //Init arm_cfft_16
    S = & arm_cfft_sR_f32_len32;
    //printf("Debug1\r\n");
	while (1)
	{
     	    read_adc_to_memory();
            //DSP read sample
     	    arm_cfft_f32(S, sample, 0, 1);
            // Calculate magnitude of complex numbers output by the FFT.
            arm_cmplx_mag_f32(sample, magnitudes, N);
            mag_to_buf();
            update_8x8_led();
          
//	    for (int i=0 ; i < N ; i++){
//		    printf("magnitude[%d] %f\r\n",i,magnitudes[i]);
 //           }
           
	 /*   for (int i=0 ; i < N ; i++) {
	    	printf(
	    }
	   */ 

	}
}
