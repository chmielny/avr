#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define ENA PB0
#define DATA PB1
#define CLK PB2

volatile unsigned char dataframe[14];
volatile unsigned int iter;
volatile unsigned int sender_busy;

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
	int i;
	for(i=0;i<7;++i)
		dataframe[i+7] = dataframe[i];
	iter = 0;
	PORTB |= (1 << ENA);
	_delay_ms(0.1);
	PORTB &= ~(1 << ENA);
	_delay_ms(0.1);
	PORTB |= (1 << ENA);
	sender_busy = 1;
	TCCR1B |= (1 << CS10);		// bez preskalera
	TIMSK |= (1 << OCIE1A);		// przerwanie na porownanie
	
}

int main(void)
{
	sei();
	PORTB = 0;
	
	_delay_ms(1000);
	fis_start();

	dataframe[0] =8;
	dataframe[1] =0;
	dataframe[2] =32;
	dataframe[3] =32;
	dataframe[4] =32;
	dataframe[5] =32;
	dataframe[6] =0;
	fis_send_frame();
	
	while(sender_busy);	
	_delay_ms(5);
	dataframe[2] =49;
	dataframe[3] =48;
	dataframe[4] =51;
	dataframe[5] =53;
	fis_send_frame();
	while(sender_busy);	
	_delay_ms(5);
	fis_send_frame();

	while (1) {
	}
}


ISR (TIMER1_COMPA_vect) {
	if(iter < 112) {
		PORTB ^= (1 << CLK);
		if(!(iter % 2)) {
			if ((dataframe[iter / 16] & (128 >> (iter/2) % 8)))
				PORTB &= ~(1 << DATA);
			else
				PORTB |= (1 << DATA);
		}
	} else if (iter >= 112 && iter <= 114) {
		PORTB &= ~(1 << ENA);
		// czekamy
	} else if (iter > 114 && iter <= 226) {
		PORTB |= (1 << ENA);
		PORTB ^= (1 << CLK);
		if(!((iter - 114) % 2)) {
			if ((dataframe[(iter - 114) / 16] & (128 >> ((iter - 114)/2) % 8)))
				PORTB &= ~(1 << DATA);
			else
				PORTB |= (1 << DATA);
		}
	} else {
		TIMSK &= ~(1 << OCIE1A);		// wyl. przerwanie na porownanie
		TCCR1B &= ~(1 << CS10);			// wyl. zegar
		PORTB &= ~(1 << ENA);
		iter = 0;
		sender_busy = 0;
	}
	iter = iter + 1;
}
