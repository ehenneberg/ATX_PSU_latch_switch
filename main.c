/*
 Simple PC ATX PSU latch switch - 2025.04.22 / EH
 Attiny25/45/85 wiring:
 One 10K resistor from pin 8 (Vcc) to pin 1 (!Reset) 
 ATX purple wire 5V to Attiny pin 8 (Vcc)
 ATX black wire to Attiny pin 4 (GND)
 ATX green wire (PS_ON) to Attiny pin 5 (PB0)
 PC power switch wires to GND and Attiny pin 3 (PB4)
*/
#define F_CPU 1E6 // CPU internal clock 1 MHz with DIV8 fuse

#include <avr/io.h>
#include <util/delay.h>

#define PS_ON_PIN PORTB0		// ATX PSU ON pin
#define PWR_SWT PORTB4			// power switch pin 
#define PSU_pwr_up_delay 500	// ATX PSU power up settling time
#define antiBounceDelay 30		// power switch anti bounce delay
#define pwrSwtTurnOffTime 1500	// pwr switch press time before PSU off

uint8_t PSUon = 0;				// ATX PSU state flag

int main(void)  {
	DDRB &= _BV(PWR_SWT);		 // set the power switch pin as an input
	PORTB |= _BV(PWR_SWT);		 // pull up resistor on power switch input
	DDRB |= _BV(PS_ON_PIN);		 // set ATX PSU control pin as an output
	PORTB |= _BV(PS_ON_PIN);	 // set ATX PSU control pin high
    _delay_ms(PSU_pwr_up_delay); // ATX PSU start settle delay
    while (1) {
		if (PSUon == 0) {		// going to turn on ATX PSU ?
			while ((PINB & _BV(PWR_SWT)) != 0);
			_delay_ms(antiBounceDelay);			// anti bounce delay
			PSUon = 1;
  	 		PORTB &=~_BV(PS_ON_PIN);
			while ((PINB & _BV(PWR_SWT)) == 0); // wait for switch release
			_delay_ms(antiBounceDelay);			// anti bounce delay			   
		}
		else {	// PSon == 1, turn off ATX PSU only after long switch press
			while ((PINB & _BV(PWR_SWT)) != 0);
			_delay_ms(antiBounceDelay);			//  anti bounce delay
			int i = pwrSwtTurnOffTime/20;	// test pwr switch increments
			while (((PINB & _BV(PWR_SWT)) == 0) && (i > 0)) {
				_delay_ms(20);
				i--;
			}
			if (i == 0) { // reached full pwr switch press time for PSU off ?
				PSUon = 0;
				PORTB |= _BV(PS_ON_PIN);			// turn off ATX PSU
				while ((PINB & _BV(PWR_SWT)) == 0); // wait for switch release
				_delay_ms(antiBounceDelay);			//  anti bounce delay
			}
			else { _delay_ms(antiBounceDelay); }	//  anti bounce delay
		}
    }
}

