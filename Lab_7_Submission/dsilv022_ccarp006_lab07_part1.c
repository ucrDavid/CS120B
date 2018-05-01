/*	Partner(s) Name & E-mail: David Silva (dsilv022@ucr.edu), Connor Carpenter (ccarp006@ucr.edu)
*	Lab Section: 024
*	Assignment: Lab 07  Exercise 01
*	I acknowledge all content contained herein, excluding template or example
*	code, is my own original work.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

enum INC_States {START, INIT, LED0, LED1, LED2, LED3, ON, OFF};
INC_States state1 = START;
INC_States state2 = START;
unsigned char sequence = 0;
unsigned char blink = 0;


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	// AVR output compare register OCR1A.
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}


void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


void Tick_Sequence() {

	switch (state1)
	{
		case START: state1 = INIT;
			break;
		case INIT: state1 = LED1;
			break;
		case LED1: state1 = LED2;
			break;
		case LED2: state1 = LED3;
			break;
		case LED3: state1 = LED1;
			break;
		default: state1 = START;
			break;
	}
	
	switch (state1)
	{
		case START://do nothing
			break;
		case INIT: 
			break;
		case LED1:  sequence = 0x01;
			break;
		case LED2: sequence = 0x02;
			break;
		case LED3: sequence = 0x04;
			break;
		default:
			break;
	}

	
}

void Tick_Blink() {

	switch (state2)
	{
		case START: state2 = INIT;
			break;
		case INIT: state2 = ON;
			break;
		case ON: state2 = OFF;
			break;
		case OFF: state2 = ON;
			break;
		default: state2 = START;
			break;
	}
	
	switch (state2)
	{
		case START:
			break;
		case INIT: blink = 0;
			break;
		case ON: blink = 0x08;
			break;
		case OFF: blink = 0;
			break;
		default:
			break;
	}

	
}

void Tick_Combine() {
	PORTA = blink | sequence;	
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	TimerSet(1000);
	TimerOn();
	/* Replace with your application code */
	while (1)
	{
		Tick_Sequence();
		Tick_Blink();
		Tick_Combine();
		while (!TimerFlag){}
			
		TimerFlag = 0;
	}
}