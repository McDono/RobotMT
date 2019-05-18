/*
Club Robot - Polytech Marseille

Fonctions li�es � l'utilisation du module UART

version 0.0.1: 21/01/2011


R�vision:     	$Rev: $
Auteur:       	$Author: $
Date r�vision: 	$Date:  $
projet:      	$URL: $

*/
#ifndef _ASSIGN_H_
#define _ASSIGN_H_



struct Vitesse;  // Structure contenant les vitesses re�u via UART a envoy� via PWM
{
		double Vts_M1;  // Vitesse MOTOR 1
		double Vts_M2;  // Vitesse MOTOR 
		int Flag_Vt;    // Flag permettant de distinguer les deux vitesses �crites apr�s VTSM en UART
		int Flag_Stop;
};

extern struct Vitesse Consigne;

extern void assign(char UxRx_string[UxRx_length]);
extern int assig_Vitesse(char UxRx_string[UxRx_length]);
extern void Initialisation();

#endif
