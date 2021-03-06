/*	Partner(s) Name & E-mail: David Silva (dsilv022@ucr.edu), Connor Carpenter (ccarp006@ucr.edu)
 *	Lab Section: 024
 *	Assignment: Lab 03  Exercise 03
 *	Exercise Description: [optional - include for your own benefit]
 *		A household has a digital combination deadbolt lock system on the doorway. The system
		has buttons on a keypad. Button 'X' connects to PA0, 'Y' to PA1, and '#' to PA2. Pressing
		and releasing '#' , then pressing 'Y' , should unlock the door by setting PB0 to 1. Any
		other sequence fails to unlock. Pressing a button from inside the house (PA7) locks the
		door (PB0=0). For debugging purposes, give each state a number, and always write the
		current state to PORTC (consider using the enum state variable). Also, be sure to check
		that only one button is pressed at a time.
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum LCK_States {LCK_SMStart, LCK_wait, LCK_pound, LCK_waitY, LCK_unlock, LCK_lock} LCK_State;

void TickFct_LckToggle() {
	switch(LCK_State) {
		case LCK_SMStart:
			LCK_State = LCK_wait;
			break;
		case LCK_wait:
			if (PINA == 0x04) {
				LCK_State = LCK_pound;
			}
			break;
		case LCK_pound:
			if (PINA == 0x00) {
				LCK_State = LCK_waitY;
			} else if (PINA == 0x04) {
				LCK_State = LCK_pound;
			} else {
				LCK_State = LCK_wait;
			}
			break;
		case LCK_waitY:
			if (PINA == 0x02) {
				LCK_State = LCK_unlock;
			} else if (PINA == 0x00) {
				LCK_State = LCK_waitY;
			} else {
				LCK_State = LCK_wait;
			}
			break;
		case LCK_unlock:
			if (PINA == 0x80) {
				LCK_State = LCK_lock;
			} else {
				LCK_State = LCK_unlock;
			}
			break;
		case LCK_lock:
			LCK_State = LCK_wait;
			break;
	}
	switch(LCK_State) {
		case LCK_SMStart:
			PORTC = LCK_SMStart;
			break;
		case LCK_wait:
			PORTC = LCK_wait;
			break;
		case LCK_pound:
			PORTC = LCK_pound;
			break;
		case LCK_waitY:
			PORTC = LCK_waitY;
			break;
		case LCK_unlock:
			PORTB = 0x01;
			PORTC = LCK_unlock;
			break;
		case LCK_lock:
			PORTB = 0x00;
			PORTC = LCK_lock;
			break;
		default:
			//do nothing
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0x00; PORTC = 0xFF;
    /* Replace with your application code */
    while (1) 
    {
		TickFct_LckToggle();
    }
}

