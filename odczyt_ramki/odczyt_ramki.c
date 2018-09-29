#include "HD44780.h"
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>


volatile int pozycja, znak[8];

SIGNAL(SIG_INTERRUPT0)
{
	if(pozycja<8)
	{
		znak[pozycja] = ((PIND &0b00001000) == 0b00001000);
	}
	pozycja++;
}



int main()
{
	pozycja = 0;
	char tekst[16] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
	znak[0] = 0;
	znak[1] = 0;
	znak[2] = 0;
	znak[3] = 0;
	znak[4] = 0;
	znak[5] = 0;
	znak[6] = 0;
	znak[7] = 0;

	DDRD = 0b00000000;
	PORTD = 0b11111111;

	MCUCR = 0b00000011;
	GICR = 0b01000000;
	sei();
	
	while(1)
	{	
		tekst[0] = pozycja + 48;
		
		tekst[2] = znak[7] +48;
		tekst[3] = znak[6] +48;
		tekst[4] = znak[5] +48;
		tekst[5] = znak[4] +48;
		tekst[6] = znak[3] +48;
		tekst[7] = znak[2] +48;
		tekst[8] = znak[1] +48;
		tekst[9] = znak[0] +48;
		if(pozycja>8)
			tekst[10] = znak[0] + 2*znak[1] + 4*znak[2] + 8*znak[3] + 16*znak[4] + 32*znak[5] + 64*znak[6] + 128*znak[7];
		LCD_Initalize();
		LCD_Clear();
		LCD_Home();
		LCD_WriteText(&tekst);
	
	}
	return 0;
}

