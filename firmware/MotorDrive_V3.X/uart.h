/*
Club Robot - Polytech Marseille

Fonctions li�es � l'utilisation du module UART

version 0.0.1: 21/01/2011


R�vision:     	$Rev: $
Auteur:       	$Author: $
Date r�vision: 	$Date:  $
projet:      	$URL: $

*/
#ifndef _UART_H_
#define _UART_H_


#define UxRx_length 8
#define UxTx_length 20


extern void initUART1(unsigned long);
void U1Tx_chaine(char string[UxTx_length]);

#endif
