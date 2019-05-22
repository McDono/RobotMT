import ReconnaissanceV2 as RV2
import serial
import UART
import constantes
import Mouvement
import ObjetReconnu
from time import sleep

def aligner_robot(Objet_Tracke, portSerie):

	if abs(Objet_Tracke.milieu_X) > constantes.TOLERANCE_ANGLE : #On s'aligne avec l'objet

		vitesse_correction = constantes.KA*Objet_Tracke.milieu_X

		if Objet_Tracke.milieu_X > 0 :
			#on tourne vers la droite (a verifier)
			#(vitesse_roue_droite, vitesse_roue_gauche) = (vitesse_correction, -vitesse_correction)
			Mouvement.tourner_sur_place(vitesse_correction, "droite", portSerie)
		else:
			#on tourne vers la gauche (a verifier)
			#(vitesse_roue_droite, vitesse_roue_gauche) = (-vitesse_correction, vitesse_correction)
			Mouvement.tourner_sur_place(vitesse_correction, "gauche", portSerie)
		sleep(constantes.DELAIS_ALIGNEMENT) #On laisse le temps au robot de tourner
		return 0 #le robot n'etait pas aligné à l'appel de la fonction, il a été corrigé depuis mais on ne sait pas encore si cela est suffisant
	else:
		return 1 #le robot est aligné à l'appel de la fonction

def positioner_robot(Objet_Tracke, ordre, portSerie):

	if ordre == "suivre":
		consigne = constantes.DISTANCE_SUIVIE
	else:
		consigne = constantes.DISTANCE_FUITE

	delta_D = Objet_Tracke.ratio - consigne

	#On compare la taille de l'objet par rapport à la taille
	#de l'ecran afin de savoir si le robot doit avance ou reculer
	#afin de respecter la consigne de distance (variant en fonction de l'ordre)
	if abs(delta_D) > constantes.TOLERANCE_DISTANCE: #On ajuste la position du robot

		vitesse_correction = constantes.KP*delta_D

		if delta_D < 0: #le robot est trop loin
			Mouvement.avancer(vitesse_correction, portSerie)
		else:
			Mouvement.reculer(vitesse_correction, portSerie)

		sleep(constantes.DELAIS_POSITIONNEMENT)


"""def aligner_positionner_robot(Objet_Tracke, portSerie):
    
        tourner = ""
        
        if Objet_Tracke.nom == nomObjetASuivre :
            ordre = "suivre"
        else : #si ce n'est pas l'objet à suivre, c'est l'objet à suivre au vu d'un if precedent
            ordre = "fuir"
            
        if abs(Objet_Tracke.milieu_X) > constantes.TOLERANCE_ANGLE : #On s'aligne avec l'objet

            vitesse_alignement = constantes.KA*Objet_Tracke.milieu_X

            if Objet_Tracke.milieu_X > 0 :
			#on tourne vers la droite (a verifier)
			#(vitesse_roue_droite, vitesse_roue_gauche) = (vitesse_correction, -vitesse_correction)
                tourner = "droite"
            else:
			#on tourne vers la gauche (a verifier)
			#(vitesse_roue_droite, vitesse_roue_gauche) = (-vitesse_correction, vitesse_correction)
                tourner = "gauche"
            

    	#On compare la taille de l'objet par rapport à la taille
    	#de l'ecran afin de savoir si le robot doit avance ou reculer
    	#afin de respecter la consigne de distance (variant en fonction de l'ordre)
        if abs(delta_D) > constantes.TOLERANCE_DISTANCE: #On ajuste la position du robot

            vitesse_position = constantes.KP*delta_D

            if delta_D < 0: #le robot est trop loin
                Mouvement.tourner_et_avancer(vitesse_alignement, vitesse_position, direction, portSerie)
                Mouvement.reculer(vitesse_correction, portSerie)"""

    
    