#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#define ENA PB0
#define DATA PB1
#define CLK PB2
#define ENA_IN PD2
#define DATA_IN PD1
#define CLK_IN PD3

volatile unsigned char dataframe[14];
volatile uint8_t iter;
volatile bool sender_busy;
volatile bool fis_run;

volatile unsigned char in_dataframe[18];
volatile uint8_t iter_in;
volatile uint8_t ena;

//test
char str[4];

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

ISR(INT0_vect)		// przerwanie od ENA IN	
{
	if(ena == 0) {
		ena++;
		GICR |= (1 << INT1);
		iter_in = 0;
	} else if(ena == 1) {
		ena = 1;
	} else if(ena == 2) {
		ena = 0;
		GICR &= ~(1 << INT1);
	}
}

ISR(INT1_vect)		// przerwanie od CLK IN
{
	if(ena == 1) ena = 2;
	in_dataframe[iter_in / 8] &= ~(1 << (7 - (iter_in % 8)));
	in_dataframe[iter_in / 8] |= (!(PIND & (1 << DATA_IN))) << (7 - (iter_in % 8));
	iter_in++;
}

void fis_start(void) {
	// ustawienie timera 1
       	TCCR1B |= (1 << WGM12);		// tryb ctc
	OCR1A = 422;			// dla takiej wartosci gran. jest ok. 9.7 kHz

	// sekwencja startowa fis
	if (fis_run == false) {		// jezeli fis nie zostal jeszcze wlaczony
		fis_run = true;
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
	}
	return;
}

void fis_send_frame(void) {
	uint8_t i;
	if (fis_run) {				// jezeli fis wlaczony
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
}

void fis_close(void) {
	if (fis_run) {
		PORTB &= ~((1 << ENA) | (1 << DATA) | (1 << CLK));
		_delay_ms(43);
		PORTB |= (1 << DATA);
		_delay_ms(2.1);
		PORTB &= ~(1 << DATA);
		PORTB |= (1 << ENA);
		_delay_ms(2.6);
		PORTB &= ~((1 << ENA) | (1 << DATA) | (1 << CLK));
		fis_run = false;
	}
}

// tryb cd, parametry: nr cd, nr piosenki
void fis_cd(char cd_number[2], char track[2]) {
	dataframe[1] =8;
	dataframe[2] =track[0];
	dataframe[3] =track[1];
	dataframe[4] =cd_number[0];
	dataframe[5] =cd_number[1];
	dataframe[6] =0;
	fis_send_frame();
		
}

// tryb radia, parametry: nr banku, nr programu, czestotliwosc, czy rds
void fis_fm(char bank, char prog, char freq[4], bool rds) {
	if(bank == '1')
		dataframe[0] =26 + rds * 64;
	else
		dataframe[0] =30 + rds * 64;
	dataframe[1] =2;	// stereo wlaczone
	dataframe[2] =freq[0];
	dataframe[3] =freq[1];
	dataframe[4] =freq[2];
	dataframe[5] =freq[3];
	dataframe[6] = ((prog - 48) << 4);
	fis_send_frame();
}



int main(void)
{
	DDRB |= (1 << ENA) | (1 << DATA) | (1 << CLK);			// piny wyjsciowe do starego fisa

	DDRD &= ~((1 << ENA_IN) | (1 << DATA_IN) | (1 << CLK_IN));	// piny wejsciowe do nowego fisa
	MCUCR |= (1 << ISC11) | (1 << ISC01);				// przerwania na zboczach opadajacych
	GICR |= (1 << INT0);

	fis_run = false;

	ena = 0;
	iter_in = 0;

	sei();
	
	while (1) {
		if((ena == 0) && (in_dataframe[0] == 240)) {
			if (in_dataframe[1] == 'C' && in_dataframe[2] == 'D' && in_dataframe[9] == 'T' && in_dataframe[10] == 'R') {
				fis_start();
				fis_cd(&in_dataframe[4], &in_dataframe[12]);
				_delay_ms(2000);
	//			fis_fm('0', '0', &in_dataframe[1], false);
				_delay_ms(2000);
				fis_close();
			}
			in_dataframe[0] = 0;
		}


	}
}
