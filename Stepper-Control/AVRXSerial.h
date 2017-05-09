/*! \file *********************************************************************
 *
 * \brief  Serial I/O library for AVR XMEGA.
 *
 *      This is a generic serial/RS-232 I/O module for the AVR XMega	
 * 		The calling code must instantiate the XUSARTst structure for the	
 *		desired serial port (8 to choose from), and must use the ISR 		
 *  	template to handle interrupts (see header file).					
 *		The USART has a separate status flag to tell the main 		
 * 		program when data is ready or has been sent.						
 *		The transmit and receive queues are implemented as circular buffers.			
 *
 * \par 
 *      
 * \author
 *      Applied Optical Engineering: http://www.aplopteng.com \n
 *      Support email: info@aplopteng.com
 *
 * $Revision:  4.0 $
 * $Date: 2015-01-04 $  \n
 *
 * Copyright (c) 2008, Applied Optical Engineering All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are not permitted.
 *
 *****************************************************************************/

#if !defined(_AVRX_SERIAL_INC)
#define _AVRX_SERIAL_INC

// USART registers and structures are defined in the io map file (device header)
// 	this file re-maps them to make USART initialization generic to the entire XMega series
//

//	this is the structure definition from the device file, for reference
//
/* Universal Synchronous/Asynchronous Receiver/Transmitter */
/* typedef struct USART_struct
{
    register8_t DATA;  // Data Register 
    register8_t STATUS;  // Status Register 
    register8_t reserved_0x02;
    register8_t CTRLA;  // Control Register A 
    register8_t CTRLB;  // Control Register B 
    register8_t CTRLC;  // Control Register C 
    register8_t BAUDCTRLA;  // Baud Rate Control Register A 
    register8_t BAUDCTRLB;  // Baud Rate Control Register B 
} USART_t;
*/

//	these definitions are easier to use than the bit-rotated versions
//	in the io.h file
//

#define _USART_RXCIL_OFF 		0x00  //!< Receive Complete Interrupt disabled
#define _USART_RXCIL_LO  		0x10   //!< Receive Complete Interrupt lo priority
#define _USART_RXCIL_MED 		0x20  //!< Receive Complete Interrupt medium priority
#define _USART_RXCIL_HI  		0x30  //!< Receive Complete Interrupt hi priority


#define _USART_TXCIL_OFF  		0x00  //!< Transmit Complete Interrupt disabled
#define _USART_TXCIL_LO 		0x04  //!< Transmit Complete Interrupt lo priority
#define _USART_TXCIL_MED 		0x08  //!< Transmit Complete Interrupt med priority
#define _USART_TXCIL_HI  		0x0C   //!< Transmit Complete Interrupt hi priority


#define _USART_DREIL_OFF		0x00  //!< Data Register Empty Interrupt disabled
#define _USART_DREIL_LO  		0x01   //!< Data Register Empty Interrupt lo priority
#define _USART_DREIL_MED  		0x02  //!< Data Register Empty Interrupt medium priority
#define _USART_DREIL_HI  		0x03  //!< Data Register Empty Interrupt hi priority
#define _USART_CLK2X			0x04  //!< turn on clock doubler

#define _USART_CHSZ_5BIT  		0x00   //!< Character size: 5 bit
#define _USART_CHSZ_6BIT  		0x01   //!< Character size: 6 bit
#define _USART_CHSZ_7BIT  		0x02   //!< Character size: 7 bit
#define _USART_CHSZ_8BIT  		0x03    //!< Character size: 8 bit
#define _USART_CHSZ_9BIT  		0x07    //!< Character size: 9 bit
#define _USART_SM_1BIT			0x00   //!< 1 stop bit
#define _USART_SM_2BIT			0x04  //!< 2 stop bits

#define _USART_PM_DISABLED  	0x00    //!< No Parity
#define _USART_PM_EVEN  		0x20    //!< Even Parity
#define _USART_PM_ODD  			0x30    //!< Odd Parity

// USART status flags

#define _USART_TX_EMPTY			0x01	//!< USART transmitter empty - Tx done
#define _USART_TX_OVF			0x02	//!< USART Tx queue overflow
#define _USART_RX_DONE			0x04	//!< Rx termination sequence detected	
#define _USART_RX_OVF 			0x08	//!< Rx buffer overflow
#define _USART_TX_ON			0x10	//!< transmitter enabled
#define _USART_RX_ON			0x20	//!< receiver enabled
#define _USART_ERROR			0x80	//!< Other error

// USART input mode flags

#define _INPUT_ECHO				0x01	//!< used for terminal echo
#define _INPUT_LF				0x02	//!< look for LINEFEED termination on input	
#define _INPUT_CR				0x04	//!< CARRIAGE_RETURN termination
#define _INPUT_CRLF				0x08	//!< look for CR/LF (or LF/CR) termination on input
#define _INPUT_TTY				0x10	//!< process incoming chars as keyboard input (BS, DEL, etc.)

// USART output mode flags

#define _OUTPUT_LF				0x01	//!< append LF to output strings
#define _OUTPUT_CR				0x02	//!< append CR to output strings
#define _OUTPUT_CRLF			0x04	//!< append CR/LF to output strings

#define _MAX_USARTQ 		254		//!< USART Q size - must be countable using 8-bit variables
#define _GETCH_WAIT			1024	//!< wait time for getchar()

#define CARRIAGE_RETURN 	0x0D
#define LINEFEED			0x0A
#define BACKSPACE			0x08

//! \brief OverwriteMode enum - describes possibilities for overflow handling
typedef enum {	OVERWRITE, 	//!< overwrite existing characters with new ones
				OVERFLOW	//!< keep existing characters, discard new ones
			} OverwriteMode;

/*! \brief XUSART structure prototype
 *	
 *	Holds register info, character buffer pointers, mode flags,
 *	termination, parity, baud, etc.
 *	
 *	Character buffers are implemented as pointers to circular buffers.
 *	Calling code must instantiate the buffers and assign the pointers.
 *
 */
volatile typedef struct {
	volatile uint16_t rDATA, rSTATUS, rCTRLA, rCTRLB, rCTRLC, rBAUDCTRLA, rBAUDCTRLB;
	volatile char 	*TxCB, 					//!< transmitter circular buffer pointer
					*RxCB, 					//!< receiver circular buffer pointer
					rch;					//!< internal (dummy) 1-byte buffer
	volatile uint8_t	uTxQnow, 			//!< current position of Tx queue pointer
						uTxQlen, 			//!< length of transmit queue (# chars to send)
						uTxQmax,			//!< maximum length of transmit buffer
						uRxQnow,  			//!< current position of Rx queue pointer
						uRxQstart,			//!< starting position of Rx queue
						uRxQend, 			//!< end of receive queue (to term char or set for limit)
						uRxQmax,			//!< length of Rx queue (max size of buffer)
						serStatus, 			//!< status flag for USART (see flags)
						fInMode, 			//!< input mode flag
						fOutMode, 			//!< output mode flag
						uID,				//!< physical ID of USART (C0, D1, E0, etc.)
						uPar, 				//!< parity mode 
						uStop,				//!< number of stop bits
						uData,				//!< data size
						uEnable;			//!< Tx/Rx enable, none or both
	volatile OverwriteMode eRxOvrMode;		//!< what to do when Rx buffer overflows
	volatile int8_t nBscale;				//!< baud rate scaling factor
	volatile uint16_t nHbaud;				//!< baud rate in hundred-baud

} XUSARTst;


/*! \brief USART Tx_Handler	function
 *	
 *	\par
 *	Output data via USART. 	
 *	Set mode flags in XUSARTst if Tx is done. 
 *  Reset counters and buffer length if done. 
 *
 *	\param *stU		pointer to XUSARTst instance
 *
 *	\par Note:
 *	ISR must be instantiated in user code (not library).  Example:
 *
 *	ISR(USART\<XX\>_TXC_vect)
 *	{
 *		Tx_Handler(\&\<XUSARTst structure\>);
 *	}
 *
 *	Substitute the number of the USART (e.g. C0, E1, etc.) for \<XX\>
 *	Substitute user variable (declared globally) for \<XUSARTst structure\>
 *
 */
void Tx_Handler(XUSARTst *stU);

/*! \brief USART Rx_Handler	function
 *
 *	Input data via USART.  If synchronous Rx desired, test for Rx 
 *	done via term char or CR/LF method.  
 *	Set mode flags in UARTstruct if Rx is done.  
 *  Reset counters and buffer length.  
 *	Check for IDLE state
 *	
 *	\param *stU		pointer to XUSARTst instance
 *
 *	\par
 *	Note:
 *	ISR must be instantiated in user code (not library).  Example:
 *
 *	ISR(USART\<XX\>_RXC_vect)
 *	{
 *		Rx_Handler(\&\<XUSARTst structure>);
 *	}
 *
 *	Substitute the number of the USART (e.g. C0, E1, etc.) for \<XX\>
 *	Substitute user variable (declared globally) for \<XUSARTst structure\>																		
 *
 */
void Rx_Handler(XUSARTst *stU);

/*!	\brief USART_putchar function
 *
 *	Send single character via selected USART
 *	
 *	\param *stU		pointer to XUSARTst instance
 *	\param c		character to be sent
 *
 *	\retval -1 		if error (e.g. TXEN not on)
 *	\retval 0		normal completion
 */
int8_t USART_putchar(XUSARTst *stU, char c);

/*!	\brief USART_getchar function
 *
 *	Read single character from selected USART.
 *  Blocking function; will disable Rx interrupt
 *		
 *	\param  *stU	pointer to XUSARTst instance
 *
 *	\retval NULL 	if error or no char ready
 *	\retval <char>	normal completion
 */
char USART_getchar(XUSARTst *stU);

/*!	\brief USART_send function
 *
 *	\par
 *	Sends string data via selected USART
 *		calculate buffer requirements, return if overflow
 *		copy & terminate target string 
 *		load data into circular Tx buffer 
 *		start Tx byte pump if needed
 *
 *	\param *stU		pointer to XUSARTst instance
 *	\param *s		pointer to character array
 *
 *	\retval -3		if string is too long for buffer
 *	\retval	-2 		if overflow (also sets serStatus flag)
 *	\retval -1 		if USART not enabled or not instantiated
 *	\retval 0 		for normal operation
 *	\retval 1 		for normal operation with Tx pump start
 *
 */
int8_t USART_send(XUSARTst *stU, const char *s);

/*!	\brief USART_read function
 * 
 *	Reads data via selected USART 
 *		flush buffer 
 *		copy string, clear flag & return 
 *
 *	\param *stU		pointer to XUSARTst instance
 * 	\param *r		pointer to character buffer to read into
 * 
 *	\retval	-1		if stU not instantiated
 *	\retval 0		normal operation
 *
 */
int8_t USART_read(XUSARTst *stU, char *r);

/*!	\brief USART_enable function
 * 
 * 	Turns Tx & Rx on or off, enables/disables interrupts 
 *	Enabled state is a bitwise mask of enables - USART_TXEN_bm and 
 *	USART_RXEN_bm 
 *		MAKE SURE code is compiled correctly to use appropriate ISRs 
 * 
 *	\param *stU		pointer to XUSARTst instance
 * 	\param nEnable	bitwise combination of USART_TXEN_bm and USART_RXEN_bm
 *
 *	\retval	-1		if stU not instantiated
 *	\retval 0		normal operation
 * 
 */
int8_t USART_enable(XUSARTst *stU, uint8_t nEnable);

/*!	\brief USART_init function
 *
 * 	Sets up baud, bits, etc. 
 *
 *	\par
 *	\param *stU		pointer to XUSARTst instance
 *	\param nUSART  	ID of USART (0xC0, 0xC1, 0xD0, 0xD1, 0xE0, 0xE1, 0xF0, or 0xF1)
 *	\param pClk  	peripheral clock frequency (see AVRX_Clocks.h)
 *	\param nIntLvl 	interrupt level: OFF, LO, MED, HI (see datasheet!)
 *	\param nhBaud  	baud rate in hundreds (e.g. 96 for 9600 baud)
 *	\param nBScale	fractional baud rate scaling factor (see datasheet!)
 *	\param nData	number of data bits (7 or 8)
 *	\param nPar 	number of parity bits (0 or 1)
 *	\param nStop	number of stop bits (0 or 1)

 * Example:
 * USART_init(&stU, 0xD0, 8000000L, (_USART_TXCIL_LO | _USART_RXCIL_LO), 576, -4,
 *		_USART_CHSZ_8BIT, _USART_PM_DISABLED, _USART_SM_1BIT);
 *
 *	\retval	-1		for invalid USART ID
 *	\retval 0		normal operation
 *
 */
int8_t USART_init(XUSARTst *stU, uint8_t nUSART, unsigned long pClk, uint8_t nIntLvl, 
	uint16_t nhBaud, int8_t nBScale, uint8_t nData, uint8_t nPar, uint8_t nStop);


/*! \brief USART_RxFlush function
 * 
 *	Flush the read queue for selected USART  
 *		flush buffer 
 *		reset counters 
 *		WARNING: all buffer data will be lost
 *
 *	\param *stU		pointer to XUSARTst instance
 *
 *	\retval	-1		if stU not instantiated
 *	\retval 0		normal operation
 * 
 */
int8_t USART_RxFlush(XUSARTst *stU);

/*!	\brief USART_buffer_init function
 *
 * 	Sets up USART character buffers (instantiated elsewhere for memory optimization). 
 *
 *	\param *stU		pointer to XUSARTst instance
 *	\param nRx		length of Rx circular buffer 
 *					(set to zero to free memory)
 *	\param nTx		length of Tx circular buffer 
 *					(set to zero to free memory)	
 *
 *	\retval	-1		if stU not instantiated
 *	\retval 0		normal operation
 *
 */
int8_t USART_buffer_init(XUSARTst *stU, uint8_t nRx, uint8_t nTx);

/*! now the nightmare!
 *	versions with 1 serial port (B3 series):
 *		64B3 - configured as C0; in avrxmega4				  				(config 1)
 *		128B3 - configured as C0; in avrxmega6
 *	
 *	versions with 2 serial ports (B1, D4, & E5 series): 
 *		16D4, 32D4, 8E5, 16E5, 32E5 - configured as C0 & D0; in avrxmega2 	(config 2A)
 *		64B1 - configured as C0 & E0; in avrxmega4 							(config 2B)
 *		64D4 - configured as C0 & D0; in avrxmega4 
 *		128B1 - configured as C0 & E0; in avrxmega6
 *		128D4 - configured as C0 & D0; in avrxmega6
 *		 - 
 *	versions with 3 serial ports (C3, C4, & D3 series):
 *		16C4,  32C4  -  configured as C0, C1, D0; in avrxmega2				(config 3A)
 *		32C3, 32D3 - configured as C0, D0, E0; in avrxmega2					(config 3B)
 *		64C3, 64D3 - configured as C0, D0, E0; in avrxmega4
 *		128C3, 192C3, 256C3, 384C3, 128D3, 192D3, 256D3, 384D3 -
 *			configured as C0, D0, E0; in avrxmega6
 *
 *	versions with 5 serial ports (A4 & A4U series):
 *		16A4(U), 32A4(U) - ports C0, C1, D0, D1, E0; in avrxmega2			(config 5)
 *		64A4U - ports C0, C1, D0, D1, E0; in avrxmega4
 *		128A4U - ports C0, C1, D0, D1, E0; in avrxmega7
 *	
 *		apparently the 64A4 & 128A4 are non-production models, even though spec'd by datasheet
 *
 *	versions with 6 serial ports (A3B & A3BU series):
 *		256A3B(U) - ports C0, C1, D0, D1, E0, F0; in avrxmega6				(config 6)
 *
 *		most of this series appears also not to exist in real silicon
 *
 *	versions with 7 serial ports (A3 & A3U series):
 *		64A3(U) - ports C0, C1, D0, D1, E0, E1, F0; in avrxmega4			(config 7)
 *		128A3(U), 192A3(U), 256A3(U) - 
 * 			ports C0, C1, D0, D1, E0, E1, F0; in avrxmega6
 *
 *	versions with 8 serial ports (A1 & A1U series):
 *		64A1(U) - ports C0, C1, D0, D1, E0, E1, F0, F1; in avrxmega5		(config 8)
 *		128A1(U) - ports C0, C1, D0, D1, E0, E1, F0, F1; in avrxmega7
 *		
*/

// the only thing that really changes is the specific registers that are mapped
// it appears all the Xmega series preserve the basic register setup and functionality
// so we only need to assign the specific registers to the generic USART port, and 
// then all the other library functions will work fine
// need prototype definition first - this is for the generic library build

int8_t assign_USART_regs(XUSARTst *, uint8_t nUSART);

// now branch by series-specific headers to map USART registers
// either Atmel or GNU has changed the preprocessor so that line continuation via \
// is only permissible if NO COMMENTS FOLLOW!
// This is a clear case for summary execution by slow bowel removal.
// Thus, we have to assume that the preprocessor can only define one of the 
// device symbols, but multiple defs will simply cause compile time errors

#if !defined (_AVRX_LIBRARY_BUILD) // prevent branching when compiling generic functions
			// B3 series - 1 serial port
#if (defined (__AVR_ATxmega64B3__) || defined (__AVR_ATxmega128B3__))			
#define _SERIAL_CONFIG_1
#endif	
			// D4 series - 2 serial ports config 2A
			// E5 series - config 2A
#if (defined (__AVR_ATxmega16D4__) || defined (__AVR_ATxmega32D4__)	\
	|| defined (__AVR_ATxmega64D4__) || defined (__AVR_ATxmega128D4__) \
	|| defined (__AVR_ATxmega8E5__) || defined (__AVR_ATxmega16E5__) \
	|| defined (__AVR_ATxmega32E5__))
#define _SERIAL_CONFIG_2A
#endif
				// B1 series - config 2B
#if defined (__AVR_ATxmega64B1__) || defined (__AVR_ATxmega128B1__)
#define _SERIAL_CONFIG_2B	
#endif
 				// C4 series - config 3A
#if defined (__AVR_ATxmega16C4__) || defined (__AVR_ATxmega32C4__)
#define _SERIAL_CONFIG_3A
#endif
				// C3 series - config 3B
				// D3 series - config 3B
#if (defined (__AVR_ATxmega32C3__) || defined (__AVR_ATxmega64C3__) \
	|| defined (__AVR_ATxmega128C3__) || defined (__AVR_ATxmega192C3__) \
	|| defined (__AVR_ATxmega256C3__) || defined (__AVR_ATxmega384C3__) \
	|| defined (__AVR_ATxmega32D3__) || defined (__AVR_ATxmega64D3__) \
	|| defined (__AVR_ATxmega128D3__) || defined (__AVR_ATxmega192D3__) \
	|| defined (__AVR_ATxmega256D3__) || defined (__AVR_ATxmega384D3__))
#define _SERIAL_CONFIG_3B	
#endif	
				// A4 series - config 5
				// A4U series - config 5
#if (defined (__AVR_ATxmega16A4__) || defined (__AVR_ATxmega32A4__) \
	|| defined (__AVR_ATxmega16A4U__) || defined (__AVR_ATxmega32A4U__) \
	|| defined (__AVR_ATxmega64A4U__) || defined (__AVR_ATxmega128A4U__)) 
#define _SERIAL_CONFIG_5		
#endif
				// A3B(U) series - config 6
#if defined (__AVR_ATxmega256A3B__) || defined (__AVR_ATxmega256A3BU__)
#define _SERIAL_CONFIG_6
#endif	
				// A3 series - config 7	
				// A3U series - config 7
#if (defined (__AVR_ATxmega64A3__) || defined (__AVR_ATxmega128A3__) \
	|| defined (__AVR_ATxmega192A3__) || defined (__AVR_ATxmega256A3__) \
	|| defined (__AVR_ATxmega64A3U__) ||	defined (__AVR_ATxmega128A3U__) \
	|| defined (__AVR_ATxmega192A3U__) || defined (__AVR_ATxmega256A3U__)) 
#define _SERIAL_CONFIG_7
#endif
				// A1 series - config 8
				// A1U series - config 8
#if (defined (__AVR_ATxmega64A1__) || defined (__AVR_ATxmega128A1__)	\
	|| defined (__AVR_ATxmega64A1U__) || defined (__AVR_ATxmega128A1U__))		
#define _SERIAL_CONFIG_8
#endif

#ifdef _SERIAL_CONFIG_1

int8_t assign_USART_regsX1(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX1(a, b)

#elif defined(_SERIAL_CONFIG_2A)

int8_t assign_USART_regsX2A(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX2A(a, b)

#elif defined(_SERIAL_CONFIG_2B)

int8_t assign_USART_regsX2B(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX2B(a, b)

#elif defined(_SERIAL_CONFIG_3A)

int8_t assign_USART_regsX3A(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX3A(a, b)

#elif defined(_SERIAL_CONFIG_3B)

int8_t assign_USART_regsX3B(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX3B(a, b)

#elif defined(_SERIAL_CONFIG_5)

int8_t assign_USART_regsX5(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX5(a, b)

#elif defined(_SERIAL_CONFIG_6)

int8_t assign_USART_regsX6(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX6(a, b)

#elif defined(_SERIAL_CONFIG_7)

int8_t assign_USART_regsX7(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX7(a, b)

#elif defined(_SERIAL_CONFIG_8)

int8_t assign_USART_regsX8(XUSARTst *, uint8_t nUSART);
#define assign_USART_regs(a, b)	assign_USART_regsX8(a, b)

#else
#error ("selected target is not in Xmega family")

#endif	// processor branching

#endif	// _AVRX_LIBRARY_BUILD

#endif	// _AVRX_SERIAL_INC