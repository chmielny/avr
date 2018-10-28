#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#define ENA PB0
#define DATA PB1
#define CLK PB2

volatile unsigned char dataframe[14];
volatile uint8_t iter;
volatile bool sender_busy;

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
		sender_busy = false;
	}
	iter = iter + 1;
}


void fis_start(void) {
	// ustawienie timera 1
       	TCCR1B |= (1 << WGM12);		// tryb ctc
	OCR1A = 422;			// dla takiej wartosci gran. jest ok. 9.7 kHz

	// sekwencja startowa fis
	DDRB |= (1 << ENA) | (1 << DATA) | (1 << CLK);
	PORTB &= ~(1 << ENA);
	PORTB |= (1 << DATA) | (1 << CLK);
	_delay_ms(4);
	PORTB &= ~((1 << DATA) | (1 << CLK));
	_delay_ms(1);
	PORTB |= (1 << DATA) | (1 << CLK);
	_delay_ms(1);
	PORTB &= ~((1 << DATA) | (1 << CLK));
	_delay_ms(75);
	PORTB |= (1 << DATA) | (1 << CLK);
	_delay_ms(37);
	PORTB |= (1 << ENA);
	_delay_ms(100);
	return;
}

void fis_send_frame(void) {
	uint8_t i;
	for(i=0;i<7;++i)
		dataframe[i+7] = dataframe[i];
	iter = 0;
	PORTB &= ~(1 << ENA);
	_delay_ms(0.05);
	PORTB |= (1 << ENA);
	_delay_ms(0.1);
	PORTB &= ~(1 << ENA);
	_delay_ms(0.1);
	PORTB |= (1 << ENA);
	sender_busy = true;
	TCCR1B |= (1 << CS10);		// bez preskalera
	TIMSK |= (1 << OCIE1A);		// przerwanie na porownanie
	
}

void fis_close(void) {
	PORTB &= ~((1 << ENA) | (1 << DATA) | (1 << CLK));
	_delay_ms(43);
	PORTB |= (1 << DATA);
	_delay_ms(2.1);
	PORTB &= ~(1 << DATA);
	PORTB |= (1 << ENA);
	_delay_ms(2.6);
	PORTB &= ~((1 << ENA) | (1 << DATA) | (1 << CLK));
}

void fis_cd(char track[2], char cd_number[2]) {
	dataframe[1] =8;
	dataframe[2] =track[0];
	dataframe[3] =track[1];
	dataframe[4] =cd_number[0];
	dataframe[5] =cd_number[1];
	dataframe[6] =0;
	fis_send_frame();
		
}


void fis_fm(char bank, char prog, char freq[4], bool rds) {
	if(bank == '1')
		dataframe[0] =26 + rds * 64;
	else
		dataframe[0] =30 + rds * 64;
	dataframe[1] =2;
	dataframe[2] =freq[0];
	dataframe[3] =freq[1];
	dataframe[4] =freq[2];
	dataframe[5] =freq[3];
	dataframe[6] = ((prog - 48) << 4);

	fis_send_frame();
		
}

int main(void)
{
	DDRB |= (1 << ENA) | (1 << DATA) | (1 << CLK);
	sei();
	
	fis_start();
	char a[2] = {'0', '1'};
	char b[2] = {'0', '2'};
	fis_cd(a, b);
	_delay_ms(2000);
	char c[4] = {'1','2','3','4'};
	fis_fm('2', '2', c, true);
	_delay_ms(2000);
	fis_close();

	while (1) {
	}
}
