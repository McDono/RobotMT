/*
Club Robot - Polytech Marseille

Fonctions li�es � l'utilisation du module PWM1
initialise PWM1H1 et PWM1H2
frequence?

R�vision:     	$Rev: $
Auteur:       	$Author: $
Date r�vision: 	$Date:  $
projet:      	$URL: $



*/
#ifndef _PWM_H_
#define _PWM_H_

#define FPWM      	40000					// pwm � 40KHz
#define P1TPERVAL   (FCY/FPWM/1)-1	// PTCKPS configur� pour prediviseur � 1 
#define T_IT_PWM	0.0004//16/40000=0.4ms


extern void init_PWM1();

#endif
