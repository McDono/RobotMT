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

void assign(char UxRx_string[UxRx_length]);

struct Vitesse;
{
		double Vts_M1;
		double Vts_M2;
		int Flag_Vt;
};

#endif
