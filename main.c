#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//Definition des commandes
#define CMD_QUIT "quit"
#define CMD_HELP "help"
#define CMD_CLEAR "clear"


//-------------------------------------------------------------------------------------- FONCTIONS


//Afficher la liste de commande
void liste_cmd() {
	printf("\n--- Liste des commandes disponibles ---\n");
	printf("LED1/2/3 ON/OFF\n");
	printf("CHENILLARD1/2/3 ON/OFF\n");
	printf("CHENILLARD FREQUENCE1/2/3\n");
	printf("q/Q/quit : Quitter l'application\n");
	printf("h/H/help : Afficher l'aide\n");
	printf("c/C/clear : Effacer le terminal\n");
	printf("------------------------------\n");
}

//------------------------------------------------------------------------------------------ MAIN

int main() {
	char commande[100];

	printf("Bienvenue dans le centre de contrôle du STM32 !\n");
	liste_cmd();

//--------------------------------------------------------------------------------------------- WHILE

	while (1) {
		printf(">");
		if (fgets(commande, sizeof(commande), stdin) == NULL) {
			break;
		}

		//Supprimer le caractère de nouvelle ligne à la fin d la commande
                commande[strcspn(commande, "\n")] = 0;

                bool commande_reconnaissance = false;


//----------------------------------------------------------------------------------------------- COMMANDE LED


		if (strncmp(commande, "LED", 3) == 0) {
            // ... (Analyse des commandes LED) ...
            		int led_number;
            		char state[4];
            		if (sscanf(commande, "LED%d %3s", &led_number, state) == 2) {
                		if (led_number >= 1 && led_number <= 3 &&
                    			(strcmp(state, "ON") == 0 || strcmp(state, "OFF") == 0)) {
                    			printf("Commande LED reconnue : LED %d %s (envoi au microcontrôleur)\n", led_number, state);
                    			commande_reconnaissance = true;
				}
				else {
                			printf("Erreur : Format de commande LED incorrect (LED<1-3> ON/OFF).\n");
				}
			}
		}

//-------------------------------------------------------------------------------------------------- COMMANDE CHENILLARD

		if (strncmp(commande, "CHENILLARD", 10) == 0) {
			int chenillard_number;
			char state[4];
			if (sscanf(commande, "CHENILLARD%d %3s", &chenillard_number, state) == 2) {
				if (chenillard_number >= 1 && chenillard_number <= 3 &&
					(strcmp(state, "ON") == 0 || strcmp(state, "OFF") == 0)) {
					printf("Commande Chenillard reconnue : CHENILLARD %d %s (envoi au microcontrôleur)\n", chenillard_number, state);
					commande_reconnaissance = true;
                		}
				else {
					printf("Erreur : Format de commande Chenillard incorrect (CHENILLARD<1-3> ON/OFF).\n");
				}
			}
		}

		if (strncmp(commande, "CHENILLARD FREQUENCE", 20) == 0) {
			int frequency_number;
			if (sscanf(commande, "CHENILLARD FREQUENCE%d", &frequency_number) == 1) {
				if (frequency_number >= 1 && frequency_number <= 3) {
					printf("Commande Fréquence Chenillard reconnue : CHENILLARD FREQUENCE %d (envoi au microcontrôleur)\n", frequency_number);
					commande_reconnaissance = true;
				}
				else {
					printf("Erreur : Numéro de fréquence Chenillard incorrect (1, 2 ou 3).\n");
				}
			}
			else {
				printf("Erreur : Format de commande Fréquence Chenillard incorrect (CHENILLARD FREQUENCE<1-3>).\n");
			}
		}


//-------------------------------------------------------------------------------------------------------COMMANDE GENERALE


		if (strcmp(commande, "q") == 0 || strcmp(commande, "Q") == 0 || strcmp(commande, CMD_QUIT) == 0) {
			printf("Fermeture de l'application.\n");
			break;
		}
		if (strcmp(commande, "h") == 0 || strcmp(commande, "H") == 0 || strcmp(commande, CMD_HELP) == 0) {
			liste_cmd();
			commande_reconnaissance = true;
		}
		if (strcmp(commande, "c") == 0 || strcmp(commande, "C") == 0 || strcmp(commande, CMD_CLEAR) == 0) {
			printf("\033[2J\033[H");
			commande_reconnaissance = true;
                }
		if (!commande_reconnaissance) {
			printf("Erreur : Commande inconnue. Tapez 'help' pour afficher la liste des commandes.\n");
                }
	}
	return 0;
}
