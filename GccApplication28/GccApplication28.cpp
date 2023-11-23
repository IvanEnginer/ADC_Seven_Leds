#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//------------------------0-----1-----2-----3-----4-----5-----6-----7----8-----9------DP
const char SEGMENTE[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x80};

volatile unsigned char segcounter = 0;
volatile int display_1 = 0, display_2 = 0;

ISR(TIMER2_OVF_vect)
{
	PORTD = 0x00;
	PORTB = ~(1 << segcounter);
	switch (segcounter)
	{
		case 0:
			PORTD = SEGMENTE[display_1 % 1000/100];
			break;
		case 1:
			PORTD = (SEGMENTE[display_1 % 100/10])|0x80;
			break;
		case 2:
			PORTD = SEGMENTE[display_1 % 10];
			break;
		case 3:
			PORTD = SEGMENTE[display_2 % 1000/100];
			break;	
		case 4:
			PORTD = SEGMENTE[display_2 % 100/10]|0x80;
			break;
		case 5:
			PORTD = SEGMENTE[display_1 % 10];					 
			break;
	}
	if ((segcounter++)>5) segcounter = 0;
}

volatile unsigned long value_1, value_2;
volatile unsigned long adc_counter;

ISR (ADC_vect)
{
	ADCSRA = 0;
	if ((ADMUX&0x0F) == 1)
	{
		ADMUX = 0;
			value_1 += ADC*24/45; 
	}
	else
	{
		ADMUX = (1 << MUX0);
			value_2 += ADC*24/45;
		adc_counter++;	
	}
ADCSRA = (1 << ADEN)|(1 << ADSC)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0)|(1 << ADIE);	
}

int main(void)
{
	DDRD = 0xFF;
	DDRB = (1 << PB5)|(1 << PB4)|(1 << PB3)|(1 << PB2)|(1 << PB1)|(1 << PB0);
		PORTD = 0x00;
		PORTB = 0x00;
		TIMSK |= (1 << TOIE2);
		TCCR2 |= (1 << CS21);
	ADCSRA = (1 << ADEN)|(1 << ADSC)|(1 << ADATE)|(1 << ADPS2)|(1 << ADPS1)|(0 << ADPS0)|(1 << ADIE);
	ADMUX = (1 << MUX0);
	_delay_ms(50);
	
	sei();
		
	while(1)
    {
        if (adc_counter > 400)
        {
			display_1 = (value_1/adc_counter);
			display_2 = (value_2/adc_counter);
			adc_counter = 0;
			value_1 = 0;
			value_2 = 0;
        }
	_delay_ms(100);
    }
}