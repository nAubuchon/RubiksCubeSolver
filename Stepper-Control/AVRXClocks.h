/************************************************************************/
/*																		*/
/*	AVRXClocks.h	--	Clock utility routines, header for AVR XMega	*/
/*																		*/
/*																		*/
/*	Author: 	David Schaafsma											*/
/*	Applied Optical Engineering											*/
/*																		*/
/*  Description: 														*/
/*																		*/
/*		see main file AVRX_Clocks.c										*/
/*																		*/
/*  Revision History:													*/
/*																		*/
/*	06/19/2009 (DS): created 											*/
/*	12/15/2014 (DS): updated for other Xmega series						*/
/*	12/30/2014 (DS): migrated to generic Xmega library structure		*/
/*																		*/
/************************************************************************/

#if !defined(_AVRX_CLOCKS_INC)
#define _AVRX_CLOCKS_INC

#define fCLK32M		32000000
#define fCLK2M		 2000000
#define fCLK32K	   	   32000
#define fCLKEXT		16000000

#define PLLSRC_32M		0x02
#define PLLSRC_EXT		0x03

// need prototypes here to avoid compile-time warnings

int GetSystemClocks(unsigned long *, unsigned long *);
int SetSystemClock(CLK_SCLKSEL_t, CLK_PSADIV_t, CLK_PSBCDIV_t);

#endif // !defined(_AVRX_CLOCKS_INC)
