/*
 * main.C
 *
 *  Created on: Apr 26, 2022
 *      Author: Mohamed Saleh
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

unsigned char displays[6] = {0, 0, 0, 0, 0, 0};
unsigned char second_passed = 0;
void display();
void init();
ISR(INT0_vect)
{
	unsigned char i;
	for(i = 0; i<6; i++){
		displays[i] &= 0;   /* RESET */
	}
}

ISR(INT1_vect)
{
	//TIMSK &= ~(1 << OCIE1A); // Resume by disabling interrupts
	TCCR1B &= ~((1<<CS10) | (1<<CS11) | (1<<CS11));
}

ISR(INT2_vect)
{
	//TIMSK |= (1 << OCIE1A); //Stop by disabling interrupts
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10); //Mode = CTC, Prescaler = 64
}

ISR (TIMER1_COMPA_vect)
{
	second_passed = 1;
}

int main(void) {
	init();
	while(1){
		if(!second_passed) display();
		else{
			second_passed &= 0;
			displays[0] = (displays[0] + 1) % 10;
			if(displays[0]) continue;
			displays[1] = (displays[1] + 1) % 6;
			if(displays[1]) continue;
			displays[2] = (displays[2] + 1) % 10;
			if(displays[2]) continue;
			displays[3] = (displays[3] + 1) % 6;
			if(displays[3]) continue;
			displays[4] = (displays[4] + 1) % 10;
			if(displays[4]) continue;
			displays[5] = (displays[5] + 1) % 10;
		}
	}

	return 0;
}
void init(){
	MCUCR = 1<<ISC01 | 1<<ISC00 | 1<<ISC10 | 1<<ISC11; // INT0 and INT1 configuration
	MCUCSR &= ~(1<<ISC2); // INT2 configuration
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);//Mode = CTC, Prescaler = 64
	PORTB |= 1<<2; // INTERNAL RESISTOR
	PORTD |= (1<<2 | 1<<3); // INTERNAL RESISTOR
	DDRC |= 0x0F; // SET PORTC AS OUTPUT
	DDRA |= 0X3F; //SET PORTA AS OUTPUT
	GICR = 1<<INT0 | 1<<INT1 | 1<<INT2; //ENABLE INTERRUPT 0
	TIMSK |= (1 << OCIE1A); // enable compare interrupt
	TCNT1 = 0;
	OCR1A = 15625;
	sei(); //ENABLE GLOBAL INTERRUPT
}
void display(){
	int i;
	for(i = 0; i<6; i++){
		PORTA = 1 << i;
		PORTC = displays[i];
		_delay_us(200);
	}
	}
