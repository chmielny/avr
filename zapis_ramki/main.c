#include <avr/io.h>
#include <avr/interrupt.h>

int i=1;

int main(void)
{
	DDRC=255;	
	PORTC = 255;
	
       	TCCR1B |= (1 << WGM12);		// tryb ctc
	TIMSK |= (1 << OCIE1A);		// przerwanie na porownanie
	TCCR1B |= (1 << CS10);		// bez preskalera
	OCR1A = 1000;

	sei();				// wlacz przerwania

	while (1) {
	}
}

ISR (TIMER1_COMPA_vect) {
	i = ~i;
	PORTC = i;
}
