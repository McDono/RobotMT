/*
Club Robot - Polytech Marseille

Fonctions li�es � l'utilisation du module UART (sans DMA)




R�vision:     	$Rev: $
Auteur:       	$Author: $
Date r�vision: 	$Date:  $
projet:      	$URL: $



changement:
0.0.3: (todo)
	- utilisation du DMA (pour compenser le buffer overflow)
	- g�rer le buffer overflow (acquittement � faire sinon URAT bloquer)
	- prochaine �tape (besoin asservissement): 
		* ajout commande avance((signed)distance,(signed)orientation)
		* ajout commande stop, ...
0.0.2: 28/01/2011
	- correction bug consigne PWM : PWM#1 
	- initUart1() prend maintenant la vitese en baud comme param�tre
	- s�paration fct UART du main
0.0.1: 18/11/2010

v�rification ok :
- petit probl�me si message printf trop long -> buffer overflow (utiliser DMA?)

*/
#include "header.h"	// processor, constantes,
#include "uart.h"
#include "uartassig.h"
#include "timer.h"
#include "asserv.h"
#include "move.h"
#include <ctype.h>
#include <stdio.h>            /* for printf */ 
#include <stdlib.h>           /* for atoi */

struct Struct_FlagUart FlagUart;
int U1Tx_size=12;
int U2Tx_size=5;

/* variables globales */
char U1Tx_string[UxTx_length] = "ProjetUart1\n";

// Proleme au niveau du Uart2 : seul 5 caracteres passe correctement
char U2Tx_string[UxTx_length] = "ver\n";


void initUART1(unsigned long baudrate) {
	#ifndef FCY	// test si FCY est d�finit (cf header.h)
	// FCY pas d�finit, d�clenche une erreur de compilation car on ne peut pas calculer le baudrate
	#error "unknow constant FCY"
	#endif

	// Configuration of UART 
	U1BRG =  ((FCY/baudrate)/16)-1;	// Initialization of baud rate
	// configure U1MODE
	U1MODEbits.STSEL = 0;	// One Stop Bit
	U1MODEbits.PDSEL = 0;	// 8bit, No Parity
	U1MODEbits.BRGH = 0;	// 16 clocks per bit period
	U1MODEbits.ABAUD = 0;	// No Autobaud (would require sending '55')
	U1MODEbits.RTSMD = 0;	// Simplex Mode
	U1MODEbits.LPBACK = 0;	// No Loop Back		//cleared by hardware upon completion
    U1MODEbits.UEN = 0;		// TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE = 0;	// No Wake up (since we don't sleep here)
	U1MODEbits.IREN = 0;	// No IR translation
	U1MODEbits.USIDL = 1;	// disContinue in Idle
	U1MODEbits.URXINV = 0;	// 1 = UxRX Idle state is �0�
							// 0 = UxRX Idle state is �1�
	// configure U1STA
	U1STAbits.URXISEL0 = 0;	// Interrupt Reception mode : character recieved
	U1STAbits.URXISEL1 = 0;
	U1STAbits.UTXISEL1 = 1;	// Interrupt Transmission mode :
	U1STAbits.UTXISEL0 = 0;	// character is transferred to U1TSR and U1TXREG becomes empty
	U1STAbits.UTXEN = 1;	// TX pins controlled by uart 0
	U1STAbits.UTXBRK = 0;	// Disabled
	U1STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U1STAbits.UTXINV = 0;	//
	U1STAbits.ADDEN = 0;	// Address Detect Disabled

	// Interrupts : Reception
	IPC2bits.U1RXIP = 1;	// interrupt priority 3
	IEC0bits.U1RXIE = 1; 	// Enable Recieve Interrupts 1
	IFS0bits.U1RXIF = 0; 	// Clear the Recieve Interrupt Flag
	// Interrupts : Transmission
	IPC3bits.U1TXIP = 1;	// interrupt priority 2




	FlagUart.U1Tx=0;
	IEC0bits.U1TXIE	= 1;	// // Enable Transmisssion Interrupts 1

	IFS0bits.U1TXIF = 0; 	// Clear the Transmission Interrupt Flag
	
	U1MODEbits.UARTEN = 1;	// And turn the peripheral on
	U1STAbits.UTXEN = 1;	// UART1 transmitter enabled;
}


// routine d'interruption sur reception UART1
void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) {	
	static char UxRx_string[UxRx_length];
	char UxRx_char;
	static int i=0;
	IFS0bits.U1RXIF = 0;	// acquittement interruption

	// get the data 
	UxRx_char = U1RXREG;

	// check for receive errors
	if(U1STAbits.FERR == 1)
	{
		UxRx_char='b';
		U1STAbits.FERR = 0;
	}
	// must clear the overrun error to keep uart receiving 
	if(U1STAbits.OERR == 1)
	{
		UxRx_char='a';
		U1STAbits.OERR = 0;
	}

//	UxRx_string[i]=UxRx_char;
	switch (isspace(UxRx_char))
	{
		case 1:
			assig_char(UxRx_string);
			for(i=0; i<UxRx_length; i++) UxRx_string[i]=0; // nettoyage de la chaine de caract�re
			i=0;
			break;
		default :
			UxRx_string[i]=UxRx_char;
			i++;
			break;
	} 
}

void startU1TX(void){
	U1TXREG=0;			// Reveille de la ligne pas l'envoie d'un 0. L'envoie des donn�s est g�r� en interruption
}


/******************************************************************************** 
routine d'interruption transmission UART1 (buffer vide)
*/
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) 
{
	static int i = 0;
	IFS0bits.U1TXIF = 0;	// acquittement
	U1TXREG = U1Tx_string[i]; // Transmit one character
	i++;
	if(i>=U1Tx_size)
	{
	IEC0bits.U1TXIE	= 0;	// // Disenable Transmisssion Interrupts 1
	//U1STAbits.UTXEN = 0;	// UART1 transmitter Disenable;
		//FlagUart.U1Tx=1;		// Information envoye
		i=0;
	}
}



/********************************************************************************
interruption UART1 error 
*/
/*
void __attribute__ ((interrupt, no_auto_psv)) _U1ErrInterrupt(void)
{
	IFS bits.
}
*/



void initUART2(unsigned long baudrate) 
{
	#ifndef FCY	// test si FCY est d�finit (cf header.h)
	// FCY pas d�finit, d�clenche une erreur de compilation car on ne peut pas calculer le baudrate
	#error "unknow constant FCY"
	#endif

	// Configuration of UART 
	U2BRG =  ((FCY/baudrate)/16)-1;	// Initialization of baud rate

	// configure U1MODE
	U2MODEbits.STSEL = 0;	// One Stop Bit
	U2MODEbits.PDSEL = 0;	// 8bit, No Parity
	U2MODEbits.BRGH = 0;	// 16 clocks per bit period
	U2MODEbits.ABAUD = 0;	// No Autobaud (would require sending '55')
	U2MODEbits.RTSMD = 0;	// Simplex Mode
	U2MODEbits.LPBACK = 0;	// No Loop Back		//cleared by hardware upon completion
    U2MODEbits.UEN = 0;		// TX,RX enabled, CTS,RTS not
	U2MODEbits.WAKE = 0;	// Wake-up is disenabled
	U2MODEbits.IREN = 0;	// No IR translation
	U2MODEbits.USIDL = 1;	// The module continues to operate in Idle mode and provides full functionality.
	U2MODEbits.URXINV = 0;	// 1 = UxRX Idle state is �0�
							// 0 = UxRX Idle state is �1�
	// configure U1STA
	U2STAbits.URXISEL0 = 0;	// Interrupt Reception mode : character recieved
	U2STAbits.URXISEL1 = 0;
	U2STAbits.UTXISEL1 = 1;	// Interrupt Transmission mode :
	U2STAbits.UTXISEL0 = 0;	// character is transferred to U2TSR and U2TXREG becomes empty
	U2STAbits.UTXEN = 1;	// TX pins controlled by uart 0
	U2STAbits.UTXBRK = 0;	// Sync Break transmission is disabled or completed
	U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXINV = 0;	//
	U1STAbits.ADDEN = 0;	// Address Detect Disabled

	// Interrupts : Reception// Interrupts : Reception
	IPC7bits.U2RXIP = 1;	// interrupt priority 3
/*
	IEC1bits.U2RXIE = 0; 	// Disenable Recieve Interrupts 1
	FlagUart.U2Tx=1;
	FlagUart.U2Tx_wait=0;
*/
	IEC1bits.U2RXIE = 1; 	// Enable Recieve Interrupts 1
	FlagUart.U2Tx=0;
	FlagUart.U2Tx_wait=0;
/* */
	IFS1bits.U2RXIF = 0; 	// Clear the Recieve Interrupt Flag

	// Interrupts : Transmission
	IPC7bits.U2TXIP = 1;	// interrupt priority 2



	IEC1bits.U2TXIE	= 1;	// Enable Transmisssion Interrupts 
	IFS1bits.U2TXIF = 0; 	// Clear the Transmission Interrupt Flag



	U2MODEbits.UARTEN = 1;	 	// And turn the peripheral on
	U2STAbits.UTXEN = 1;	// UART1 transmitter enabled;
}


/********************************************************************************* 
routine d'interruption transmission UART2 (buffer vide)
*/
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) 
{
	static int i = 0, k=0, finenvoi=0;
	
	IFS1bits.U2RXIF = 0;	// acquittement

	
	if(FlagUart.U2Tx_wait==0){
		if(finenvoi==1){
			FlagUart.U2Tx=1; // Information envoye
			IEC1bits.U2TXIE	= 0;// Disenable Transmisssion Interrupts 2
			finenvoi=0;
		}
		if(FlagUart.U2Tx==0){
			i++;
			if(i<5){
				U2TXREG = U2Tx_string[k]; // Transmit one character
				k++;
			}
			else{
//				IEC1bits.U2TXIE	= 0;// Disenable Transmisssion Interrupts 2
				i=0;
				FlagUart.U2Tx_wait=1; // Demande d'attend avant l'envoie de nouvelle donnees	
			}

			if(k>=U2Tx_size){
				FlagUart.U2Tx_wait=1; // Demande d'attend avant l'envoie de nouvelle donnees	
				i=0;
				k=0;
				finenvoi=1;
			}
		}
	}	
}


/********************************************************************************* 
routine d'interruption sur reception UART2
*/
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) 
{

	static char UxRx_string[UxRx_length];
	char UxRx_char;
	static int i=0;
	IFS1bits.U2RXIF = 0;	// acquittement

	// get the data 
	UxRx_char = U2RXREG;

	// check for receive errors
	if(U2STAbits.FERR == 1)
	{
		UxRx_char='b';
		U2STAbits.FERR = 0;
	}
	// must clear the overrun error to keep uart receiving 
	if(U2STAbits.OERR == 1)
	{
		UxRx_char='a';
		U2STAbits.OERR = 0;
	}

	UxRx_string[i]=UxRx_char;
	switch (isspace(UxRx_char))
	{
		case 1:
			i=0;
			assig_char(UxRx_string);
			break;
		default :
			i++;
			break;
	} 
	
}


/*********************************************************************************
interruption UART2 error 
*/
/*
void __attribute__ ((interrupt, no_auto_psv)) _U2ErrInterrupt(void)
{
	IFS bits.
}
*/