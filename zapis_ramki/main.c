#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define ENA PC0
#define DATA PC1
#define CLK PC2

volatile unsigned char dataframe[7];
volatile unsigned char iter;

void fis_start(void) {
	// ustawienie timera 1
       	TCCR1B |= (1 << WGM12);		// tryb ctc
	TIMSK |= (1 << OCIE1A);		// przerwanie na porownanie
	TCCR1B |= (1 << CS10);		// bez preskalera
	OCR1A = 422;			// dla takiej wartosci gran. jest ok. 9.7 kHz

	// sekwencja startowa fis
	DDRC |= (1 << ENA) | (1 << DATA) | (1 << CLK);
	PORTC &= ~(1 << ENA);
	PORTC |= (1 << DATA) | (1 << CLK);
	_delay_ms(4.2079);
	PORTC &= ~((1 << DATA) | (1 << CLK));
	_delay_ms(65.3575);
	PORTC |= (1 << DATA) | (1 << CLK);
	_delay_ms(50.57);
	return;
}

void fis_send_frame(void) {
	iter = 0;
	sei();
}

int main(void)
{
//	DDRC=255;	
	PORTC = 0;
/*	
       	TCCR1B |= (1 << WGM12);		// tryb ctc
	TIMSK |= (1 << OCIE1A);		// przerwanie na porownanie
	TCCR1B |= (1 << CS10);		// bez preskalera
	OCR1A = 422;			// dla takiej wartosci gran. jest ok. 9.7 kHz

	sei();				// wlacz przerwania
*/
	
	_delay_ms(1000);
	fis_start();

	dataframe[0] ='a';
	dataframe[1] ='b';
	dataframe[2] ='c';
	dataframe[3] ='d';
	dataframe[4] ='e';
	dataframe[5] ='f';
	dataframe[6] ='g';

	fis_send_frame();
	while (1) {
	}
}


ISR (TIMER1_COMPA_vect) {
	PORTC ^= (1 << CLK);
//(dataframe[iter / 8] & (1 << iter % 8)) >> (iter % 8)
	PORTC |= ((dataframe[iter / 8] & (1 << iter % 8)) >> (iter % 8)) << DATA; 	
	PORTC &= ~(((dataframe[iter / 8] & (1 << iter % 8)) >> (iter % 8)) << DATA); 	
	if(iter >= 55) {
		cli();
		iter = 0;
	}
}
