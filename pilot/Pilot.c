#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


volatile short int T1[24] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
volatile short int T2[46] = {1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0};
volatile short int T3[29] = {0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0};
volatile short int T4[29] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0};
volatile short int T5[29] = {0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0};
volatile short int T6[29] = {1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0};
volatile short int T7[29] = {0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1};
volatile short int T8[29] = {1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1};
volatile short int T9[18] = {0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1};
//T10 78 -  zer
short int T11[15] = {1,0,1,0,0,0,1,0,1,0,1,0,1,0,1};
short int T12[17] = {1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1};
//T13 1259 - zer 

volatile short int i = 0;	//licznik do tablic

void vol_up(void);
void vol_down(void);
void right(void);
void left(void);
void up(void);
void down(void);


int main(void)
{
	//KONFIGURACJA PORTOW
	DDRD	= 0b00000000;	//port d - wejscie
	DDRC	= 0b01111111;	//port c - wyjscie

	PORTD	= 0b11111111;	//podciagniecie d do vcc
	PORTC	= 0b00000000;	//na poczatku zero na c

	//KONFIGURACJA LICZNIKA
	TCCR0	|= (1<<CS00) | (1<<CS01);	//preskaler 64
	TIMSK |= 1<<TOIE0; 					//przerwanie
	TCNT0 = 0;							//stan poczatkowy licznika

	long int tmp = 0;

	while(1) 
	{ 
		if(tmp < 80006)
			tmp = tmp + 1;
		if(bit_is_clear(PIND, PD0) && tmp > 80005)
		{
			tmp = 0;
			vol_up();
		}
		if(bit_is_clear(PIND, PD1) && tmp > 80005)
		{
			tmp = 0;
			vol_down();
		}
		if(bit_is_clear(PIND, PD2) && tmp > 80005)
		{
			tmp = 0;
			right();
		}
		if(bit_is_clear(PIND, PD3) && tmp > 80005)
		{
			tmp = 0;
			left();
		}
		if(bit_is_clear(PIND, PD4) && tmp > 80005)
		{
			tmp = 0;
			up();
		}
		if(bit_is_clear(PIND, PD5) && tmp > 80005)
		{
			tmp = 0;
			down();
		}

	}


	return 0;
}



void vol_up(void)
{
	i = 0;
	sei();
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<46)
	{
		PORTC = T2[i];

	}
	i=0;
	while(i<29)
	{
		PORTC = T3[i];

	}
	i=0;
	while(i<18)
	{
		PORTC = T9[i];

	}
	i=0;
	while(i<78)
	{
		PORTC = 0;

	}
	PORTC = 0b00000000;
	cli();
}




void vol_down(void)
{
	i = 0;
	sei();
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<46)
	{
		PORTC = T2[i];

	}
	i=0;
	while(i<29)
	{
		PORTC = T4[i];

	}
	i=0;
	while(i<18)
	{
		PORTC = T9[i];

	}
	i=0;
	while(i<78)
	{
		PORTC = 0;

	}
	PORTC = 0b00000000;
	cli();
}




void right(void)
{
	i = 0;
	sei();
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<46)
	{
		PORTC = T2[i];

	}
	i=0;
	while(i<29)
	{
		PORTC = T5[i];

	}
	i=0;
	while(i<18)
	{
		PORTC = T9[i];

	}
	i=0;
	while(i<78)
	{
		PORTC = 0;

	}
	i=0;
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<15)
	{
		PORTC = T11[i];

	}
	i=0;
	while(i<1259)
	{
		PORTC = 0;

	}
	PORTC = 0b00000000;
	cli();
}


void left(void)
{
	i = 0;
	sei();
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<46)
	{
		PORTC = T2[i];

	}
	i=0;
	while(i<29)
	{
		PORTC = T6[i];

	}
	i=0;
	while(i<18)
	{
		PORTC = T9[i];

	}
	i=0;
	while(i<78)
	{
		PORTC = 0;

	}
	i=0;
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<17)
	{
		PORTC = T12[i];

	}
	i=0;
	while(i<1259)
	{
		PORTC = 0;

	}
	PORTC = 0b00000000;
	cli();
}



void up(void)
{
	i = 0;
	sei();
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<46)
	{
		PORTC = T2[i];

	}
	i=0;
	while(i<29)
	{
		PORTC = T7[i];

	}
	i=0;
	while(i<18)
	{
		PORTC = T9[i];

	}
	i=0;
	while(i<78)
	{
		PORTC = 0;

	}
	PORTC = 0b00000000;
	cli();
}


void down(void)
{
	i = 0;
	sei();
	while(i<24)
	{
		PORTC = T1[i];

	}
	i=0;
	while(i<46)
	{
		PORTC = T2[i];

	}
	i=0;
	while(i<29)
	{
		PORTC = T8[i];

	}
	i=0;
	while(i<18)
	{
		PORTC = T9[i];

	}
	i=0;
	while(i<78)
	{
		PORTC = 0;

	}
	PORTC = 0b00000000;
	cli();
}




SIGNAL(SIG_OVERFLOW0)
{
	i = i + 1;
	TCNT0 = 185;
}

 
