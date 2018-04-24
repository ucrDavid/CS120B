/*    Connor Carpenter ccarp006@ucr.edu, David Silva dsilv022@ucr.edu
 *    Lab Section: 024
 *    Assignment: Lab 5  Exercise 2
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

enum States{START, INIT, LED1, LED2, LED3, WAIT, WAIT2, INC, DEC, VICTORY} state;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned char button = 0;
unsigned char score = 0;
const unsigned char* msg = "YOU WIN!!!";
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

void Tick(){
	button = ~PINA & 0x01;
	
	switch (state)
	{
	case START: state = INIT;
		break;
	case INIT: state = LED1;
		break;
	case LED1: state = button ? DEC : LED2;
		break;
	case LED2: state =  button ? INC : LED3;
		break;
	case LED3: state =  button ? DEC : LED1;
		break;
	case WAIT:
		if(score == 9)
			state = VICTORY;
		else
			state = button ? WAIT : WAIT2;
		break;
	case WAIT2: state = button ? LED1 : WAIT2;
		break;
	case INC: state = WAIT;
		break;
	case DEC: state = WAIT;
		break;
	case VICTORY: state = START;
	default: state = START;
		break;
	}
	
	switch (state)
	{
		case START:// do nothing
			break;
		case INIT: PORTB = 0x00;
			score = 0;
			break;
		case LED1: PORTB = 0x01;
			break;
		case LED2: PORTB = 0x02;
			break;
		case LED3: PORTB = 0x04;
			break;
		case WAIT: LCD_ClearScreen();
					LCD_Cursor(1);
					LCD_WriteData(score + '0');
			break;
		case INC: ++score;
			break;
		case DEC: if(score > 0)--score;
			break;
		case VICTORY:   LCD_ClearScreen();
						LCD_DisplayString(1, msg);
			break;
		default: //do nothing
			break;
	}
}

int main(void)
{
	state = START;
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(300);
	TimerOn();
	
	
    while (1) 
    {
		Tick();
		while (!TimerFlag){}
			TimerFlag = 0;
	
    }
}

