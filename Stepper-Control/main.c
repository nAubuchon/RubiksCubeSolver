/*
 * main.c
 * PHYS 402, Cube Cranker
 * Nick Aubuchon, Alex Nguyen
 */ 


/*** Macros ***/

//setup//
#define F_CPU 32000000UL	// clock rate
#define t_MAX 65535			// max timer (overflow) value

//USART//
#define baud 9600
#define nBScale 0

/*	These values are based on the clock at a div8 prescaler (=4MHz).
*	So with the steppers needing ~0.8ms period full steps for enough torque,
*	the period for the microstepping is (0.8ms)/16 = 50us per microstep.
*	To get 50us periods with the timer, 4,000,000*50us = 200 cycles
*	for the minimum CCA value */
#define s90 800     // number of steps for a 90 degree turn at 1/16 microstepping
#define sMAX 1000	// adjust this for acceleration
#define sMIN 200
#define stepAccel(num) sMAX-( (sMAX-sMIN)*num/s90 ) //macro for stepper acceleration

/*** includes ***/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "AVRXClocks.h"



/*** globals ***/

//general//
uint8_t volatile buttons = 0x00;

//steppers//
uint16_t volatile stepCount = 0;	// for iterating the number of steps
uint8_t volatile isRunning = 0x00;	// flag for whether the stepper is currently running (0 is off, >0 is on)
uint8_t volatile dirSelect = 0x00;
uint8_t volatile mtrSelectK = 0x00;
uint8_t volatile mtrSelectF = 0x00;
uint8_t volatile instrBuff = 0x00;

//serial//
char const s = '1'; // ready for instr signal
uint8_t volatile counter = 0;
uint8_t volatile incInstr = 0;



/*** functions ***/

// function for 90degree turns
void turn90();

// for sending direction and begin the stepping process
void motorInstr(uint8_t);



/***   ISR   ***/

//overflow, do nothing
ISR(TCC0_OVF_vect) {}

//compare
ISR(TCC0_CCA_vect)
{
	PORTK_OUT &= 21;	// turn off the step pins,
	PORTF_OUT &= 21;	//		preserve the direction pins
	
	stepCount++;		// increment the step count
	turn90();			// make another step
}

//recieve complete
ISR(USARTD0_RXC_vect)
{
    PORTH_OUT = 0xFF;
	incInstr = 0x7A - USARTD0_DATA;
    USARTD0_CTRLB &= ~(USART_RXEN_bm);
	motorInstr(incInstr);
}

//transmit complete, do nothing
ISR(USARTD0_TXC_vect){}

//pushbutton interrupt for manual control
ISR(PORTJ_INT0_vect)
{
	buttons = PORTJ_IN;
	_delay_ms(5);
	if(PORTJ_IN == buttons) {
        switch(buttons) {
            // Up
            case 0x01:
                motorInstr(0x02);
                break;
            // Right
            case 0x02:
                motorInstr(0x04);
                break;
            //Front
            case 0x04:
                motorInstr(0x08);
                break;
            // Down
            case 0x08:
                motorInstr(0x10);
                break;
            // Left
            case 0x10:
                motorInstr(0x20);
                break;
            // Back
            case 0x20:
                motorInstr(0x40);
                PORTH_OUT = 0xFF;
                break;
            // (Unreserved Function)
            case 0x40:
                //motorInstr(0);
                break;
            // (Unreserved Function)
            case 0x80:
                //motorInstr(0);
                break;
        }
	}
    //clear button buffer
	buttons = 0x00;
}



/***Stepping functions***/

//Does one microstep at a time, called by the motorInstr function and the CCA interrupt
void turn90() {
	if(stepCount < s90) {                // check step count
		PORTK_OUT |= mtrSelectK;            // set ports
		PORTF_OUT |= mtrSelectF;            /// K & F
		TCC0_CTRLA = TC_CLKSEL_DIV8_gc;     // turn on the timer
        TCC0_CCA = stepAccel(stepCount);    // set the next step's period
		TCC0_CNT = 0;                       // start the timer count over
	}
    // if it's done stepping,
	else {
		isRunning = 0;  // turn off the flag & timer, re-enable
		TCC0_CTRLA = 0; // turn off the timer
		TCC0_CCA = 0;   // turn off compare
        USARTD0_CTRLB |= USART_RXEN_bm; // re-enable USART recieve
        USARTD0_DATA = s;   // send signal for next instruction
        
        PORTH_OUT = 0x00;
	}
}

// Initiate a 90degree turn on a motor
/***************************************************************************
 Takes in the instructon mask and parses bits 0-6 to the motor instruction bytes.
 Bit 0 is the direction (each stepper's direction gets set simultaneously).
 Bits 1-6 represent the 6 different motors
 going from left to right:
    Up(W) -> 1     Right(B) -> 2      Front(R) -> 3
    Down(Y) -> 4   Left(G) -> 5       Back(O) -> 6
 
 Port K & F have three 2-bit pairs each (6 total) for Direction/Step on each motor:
    PORTK: Up(W) -> 0/1      Right(B) -> 2/3      Front(R) -> 4/5
    PORTF: Down(Y) -> 0/1    Left(G) -> 2/3       Back(O) -> 4/5
 ***************************************************************************/
void motorInstr(uint8_t instr) {
    // check if the stepper is already running
    if(isRunning == 0) {
		instrBuff = ((instr >> 0) & 1) ^ 1; // grab direction bit
        
        // Set bits 0, 2 & 4 for each port to chosen direction
		dirSelect ^= ( (-instrBuff ^ dirSelect) & (1 << 0) )
				   | ( (-instrBuff ^ dirSelect) & (1 << 2) )
				   | ( (-instrBuff ^ dirSelect) & (1 << 4) );
        
        // Grab bits 1-3
		instrBuff = (instr >> 1) & 63;
        // Set bits 1, 3 & 5 for Port K to chosen motor step
		mtrSelectK ^= ( (-((instrBuff >> 0) & 1) ^ mtrSelectK) & (1 << 1) )
					| ( (-((instrBuff >> 1) & 1) ^ mtrSelectK) & (1 << 3) )
					| ( (-((instrBuff >> 2) & 1) ^ mtrSelectK) & (1 << 5) );
        
        // Grab bits 4-6
		instrBuff = (instr >> 4) & 63;
		mtrSelectF ^= ( (-((instrBuff >> 0) & 1) ^ mtrSelectF) & (1 << 1) )
					| ( (-((instrBuff >> 1) & 1) ^ mtrSelectF) & (1 << 3) )
					| ( (-((instrBuff >> 2) & 1) ^ mtrSelectF) & (1 << 5) );
        
        //// For testing
		//PORTH_OUT = instr;
		//PORTH_OUT = (instr >> 4) & 63;
		//PORTH_OUT = ((instrBuff >> 0) & 1);
		//PORTH_OUT = mtrSelectK;
        //PORTH_OUT = mtrSelectF;
        //PORTH_OUT = dirSelect;
		
		// reset the step count, set the direction pins, makes sure step pins are off
		stepCount = 0;
        PORTK_OUT = dirSelect;
        PORTF_OUT = dirSelect;
		
		isRunning = 1;	// set the flag
        instrBuff = 0;  // clear the buffer
		turn90();		// begin turning
	}
}



/***   Setup   ***/
void setup() {
    //set clock//
    unsigned long sClk, pClk;
    SetSystemClock(CLK_SCLKSEL_RC32M_gc, CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc);
    GetSystemClocks(&sClk, &pClk);
    
    //clear & initialize//
    cli();
    
    //stepper timer//
    TCC0_PER = t_MAX;
    TCC0_CTRLA = 0;	// leave timer disabled
    TCC0_CTRLB = TC_WGMODE_SS_gc | TC0_CCAEN_bm;
    TCC0_CTRLC = 0x00;
    TCC0_CTRLD = 0x00;
    TCC0_CTRLE = 0x00;
    TCC0_CCA = 0;   // leave compare disabled
    TCC0_INTCTRLA = PMIC_LOLVLEN_bm;
    TCC0_INTCTRLB = TC0_CCAINTLVL0_bm;
    
    //USART interrupts//
    USARTD0_CTRLA = USART_RXCINTLVL_LO_gc | USART_TXCINTLVL_LO_gc;	//set low level interrupts on TX & RX
    USARTD0_CTRLB = USART_RXEN_bm | USART_TXEN_bm;	//enable TX & RX
    USARTD0_CTRLC = USART_CHSIZE_8BIT_gc;	//8 data bits, 0 parity, 1 stop
    //Baud calculation//
    unsigned long nBSel = ( (pClk)/(pow(2,nBScale) * 16 * baud) ) - 1;	//calculate BSel value
    USARTD0_BAUDCTRLA = (unsigned char)(nBSel & 0x00FF);	//bottom 8 bits of BSEL
    USARTD0_BAUDCTRLB = (char)( ((nBScale & 0x000F) << 4) | ((nBSel & 0x0F00) >> 8) );	//BSCALE and top 4 bits of BSEL
    
    //set button masks to rising edge//
    PORTJ_DIR = 0x00;
    PORTJ_INT0MASK = 0b11111111;
    PORTJ_PIN7CTRL = 1;
    PORTJ_PIN6CTRL = 1;
    PORTJ_PIN5CTRL = 1;
    PORTJ_PIN4CTRL = 1;
    PORTJ_PIN3CTRL = 1;
    PORTJ_PIN2CTRL = 1;
    PORTJ_PIN1CTRL = 1;
    PORTJ_PIN0CTRL = 1;
    PORTJ_INTCTRL = PMIC_LOLVLEN_bm;
    
    //setup port D and Q for USART//
    PORTQ_DIR = (1 << PIN2_bp); // set the enable pin
    PORTQ_OUT = (1 << PIN2_bp); // enable signal for PORTD
    PORTD_DIR = 0x08; // set tx pin to output
    
    //PORT K & F setup//
    PORTK_DIR = 0xFF;	// sets all the pins on PORT K to output
    PORTK_OUT = 0;
    PORTF_DIR = 0xFF;
    PORTF_OUT = 0;
    
    //LEDs for testing//
    PORTH_DIR = 0xFF;
    PORTH_OUT = 0;
    
    //Enable Low-Level Interrupts//
    PMIC_CTRL = PMIC_LOLVLEN_bm;
    
    //Turn on the interrupt system//
    sei();
}


/***   MAIN    ***/

int main(void)
{	
    // off to the races
	setup();
	
	while (1)	//begin while
	{		
		;
	}
}

