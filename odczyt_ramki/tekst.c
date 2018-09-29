#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include "lcd.h"


volatile int dane[144];
volatile int iter;
volatile int ena;

SIGNAL(SIG_INTERRUPT0)
{
	if(ena == 0)
	{
		ena++;
		GICR = 0b11000000;
		iter = 0;
	}
	else if(ena == 1)
	{
		ena = 1;
	}
	else if(ena == 2)
	{
		ena = 0;
		GICR = 0b01000000;
	}
}

SIGNAL(SIG_INTERRUPT1)
{
	if(ena == 1) ena = 2;
	dane[iter] = !((PIND &0b00000010) == 0b00000010);
	iter++;
}

int main(void)
{
   	DDRD = 0b00000000;
	MCUCR = 0b00001010;
    GICR = 0b01000000;
	
	int i;
	char tekst[9] = {' ',' ',' ',' ',' ',' ',' ',' ','\0'};
	char tekst2[9] = {' ',' ',' ',' ',' ',' ',' ',' ','\0'};

	char info[3];

	ena = 0;
	iter = 0;

	lcd_init();
    lcd_clear();
	
	sei();
	

    while(1)
	{
		if((ena == 0) && (dane[0]*dane[1]*dane[2]*dane[3] == 1) && (dane[4] + dane[5] + dane[6] + dane[7] == 0))
		{
//		cli();

			for(i=0;i<8;i++)
			{
				tekst[i] = (dane[15 + 8*i]) + 2*(dane[14 + 8*i]) + 4*(dane[13 + 8*i]) + 8*(dane[12 + 8*i]) + 16*(dane[11 + 8*i]) + 32*(dane[10 + 8*i]) + 64*(dane[9 + 8*i]) + 128*(dane[8 + 8*i]);
			}

			for(i=8;i<16;i++)
			{
				tekst2[i-8] = (dane[15 + 8*i]) + 2*(dane[14 + 8*i]) + 4*(dane[13 + 8*i]) + 8*(dane[12 + 8*i]) + 16*(dane[11 + 8*i]) + 32*(dane[10 + 8*i]) + 64*(dane[9 + 8*i]) + 128*(dane[8 + 8*i]);
			}

			

			_delay_ms(150);
			lcd_clear();
			lcd_text(tekst,18,0);
			lcd_text(tekst2,18,1);
			dane[0] = 0;
	//		sei();
		}
			info[0] = ena + 48;
			info[1] = iter + 48;
			info[2] = '\0';	
			lcd_text(info,3,2);
	}



   	return 0;
}



