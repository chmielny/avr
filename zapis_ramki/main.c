#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define ENA PB0
#define DATA PB1
#define CLK PB2

volatile unsigned char dataframe[7];
volatile unsigned int iter;

void fis_start(void) {
	// ustawienie timera 1
       	TCCR1B |= (1 << WGM12);		// tryb ctc
	OCR1A = 422;			// dla takiej wartosci gran. jest ok. 9.7 kHz

	// sekwencja startowa fis
	DDRB |= (1 << ENA) | (1 << DATA) | (1 << CLK);
	PORTB &= ~(1 << ENA);
	PORTB |= (1 << DATA) | (1 << CLK);
	_delay_ms(4.2079);
	PORTB &= ~((1 << DATA) | (1 << CLK));
	_delay_ms(65.3575);
	PORTB |= (1 << DATA) | (1 << CLK);
	_delay_ms(50.57);
	return;
}

void fis_send_frame(void) {
	iter = 0;
	PORTB |= (1 << ENA);
	_delay_ms(0.1);
	PORTB &= ~(1 << ENA);
	_delay_ms(0.1);
	PORTB |= (1 << ENA);
	TCCR1B |= (1 << CS10);		// bez preskalera
	TIMSK |= (1 << OCIE1A);		// przerwanie na porownanie
}

int main(void)
{
	sei();
	PORTB = 0;
	
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
	PORTB ^= (1 << CLK);
	if(!(iter % 2)) {
		if ((dataframe[iter / 16] & (1 << (iter/2) % 8)))
			PORTB &= ~(1 << DATA);
		else
			PORTB |= (1 << DATA);

//		PORTB &= ~(((dataframe[iter / 16] & (1 << (iter/2) % 8)) >> ((iter/2) % 8)) << DATA); 	
//		PORTB |= ((dataframe[iter / 16] & (1 << (iter/2) % 8)) >> ((iter/2) % 8)) << DATA; 	
	}
	iter = iter + 1;
	if(iter >= 112) {
		TIMSK &= ~(1 << OCIE1A);		// wyl. przerwanie na porownanie
		PORTB &= ~(1 << ENA);
		iter = 0;
	}
}
