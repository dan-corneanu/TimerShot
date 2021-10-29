// More info about this program is here...
// http://wp.josh.com/2015/03/05/the-perfect-pulse-some-tricks-for-generating-precise-one-shots-on-avr8/

// Demo of a technique to generate narrow and precise one shot pulses using a 
// timer module on an AVR. This demo code is writen for an Arduino and uses
// the 16 bit Timer1 moudle, but this techniquie should work on other AVRs and timers. 

// The demo generates one pulse per second. 
// The 1st pulse is 0 cycles long (no pulse), 
// The 2nd pulse is 1 cycle long (~63ns),
// The 3rd pulse is 2 cycles long (~126ns), 
// ...up to a total of 20 pulses, and then starts over.

// The one shot pulses are output on Digial pin 10



#define OSP_SET_WIDTH(cycles) (OCR1B = 0xffff-(cycles-1))

// Setup the one-shot pulse generator and initialize with a pulse width that is (cycles) clock counts long

void osp_setup(uint8_t cycles) {
  
  
  TCCR1B =  0;      // Halt counter by setting clock select bits to 0 (No clock source).
              // This keeps anyhting from happeneing while we get set up

  TCNT1 = 0x0000;     // Start counting at bottom. 
  OCR1A = 0x0000;      // Set TOP to 0. This effectively keeps us from counting becuase the counter just keeps reseting back to 0.
          // We break out of this by manually setting the TCNT higher than 0, in which case it will count all the way up to MAX and then overflow back to 0 and get locked up again.
  OSP_SET_WIDTH(cycles);    // This also makes new OCR values get loaded frm the buffer on every clock cycle. 

  TCCR1A = _BV(COM1B1) | _BV(COM1B0) | _BV(WGM11) | _BV(WGM10); // OC1B=Set on Match, clear on BOTTOM. Mode 15 Fast PWM.
  TCCR1B = _BV(WGM13)  | _BV(WGM12)  | _BV(CS11);         // Start Timer 1 with prescaler=8. Tick every 0.5µs.

  DDRB |= _BV(DDB2);     // Set pin to output (Note that OC1B = GPIO port PB2 = Arduino Digital Pin 10)
}

// Setup the one-shot pulse generator

void osp_setup() {

  osp_setup(1);

}

// Fire a one-shot pulse. Use the most recently set width. 

#define OSP_FIRE() (TCNT1 = OCR1B - 1)

// Test there is currently a pulse still in progress

#define OSP_INPROGRESS() (TCNT1>0)

// Fire a one-shot pusle with the specififed width. 
// Order of operations in calculating m must avoid overflow of the unint8_t.
// TCNT2 starts one count lower than the match value becuase the chip will block any compare on the cycle after setting a TCNT. 

#define OSP_SET_AND_FIRE(cycles) {uint16_t m=0xffff-(cycles-1); OCR1B=m;TCNT1 =m-1;}


void setup()
{
  osp_setup();

}

void loop()
{
  // Step though 0-19 cycle long pulses for demo purposes 

  for (uint8_t o = 0; o < 50; o++) {
    
    OSP_SET_AND_FIRE(o);

    while (OSP_INPROGRESS());         // This just shows how you would wait if nessisary - not nessisary in this application. 

    _delay_ms(1000);      // Wait a sec to let the audience clap

  }


}
