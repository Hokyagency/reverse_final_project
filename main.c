#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//Definition des commandes
#define LED_ON_1 "LED1 ON"
#define LED_OFF_1 "LED1 OFF"
#define LED_ON_2 "LED2 ON"
#define LED_OFF_2 "LED2 OFF"
#define LED_ON_3 "LED3 ON"
#define LED_OFF_3 "LED3 OFF"
#define CHENILLARD_ON_1 "CHENILLARD1 ON"
#define CHENILLARD_OFF_1 "CHENILLARD1 OFF"
#define CHENILLARD_ON_2 "CHENILLARD2 ON"
#define CHENILLARD_OFF_2 "CHENILLARD2 OFF"
#define CHENILLARD_ON_3 "CHENILLARD3 ON"
#define CHENILLARD_OFF_3 "CHENILLARD3 OFF"
#define CHENILLARD_FREQ_1 "CHENILLARD FREQUENCE1"
#define CHENILLARD_FREQ_2 "CHENILLARD FREQUENCE2"
#define CHENILLARD_FREQ_3 "CHENILLARD FREQUENCE3"
#define CMD_QUIT "quit"
#define CMD_HELP "help"
#define CMD_CLEAR "clear"

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

int main() {
	char commande[100];

	printf("Bienvenue dans le centre de contrôle du STM32 !\n");
	liste_cmd();


	while (1) {
		printf(">");
		if (fgets(commande, sizeof(commande), stdin) == NULL) {
			break;
		}

		//Supprimer le caractère de nouvelle ligne à la fin d la commande
		commande[strcspn(commande, "\n")] = 0;

		bool commande_reconnaissance = false;

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
		if (strcmp(commande, LED_ON_1) == 0 || strcmp(commande, LED_OFF_1) == 0 || strcmp(commande, LED_ON_2) == 0 || strcmp(commande, LED_OFF_2) == 0 || strcmp(commande, LED_ON_3) == 0 || strcmp(commande, LED_OFF_3) == 0) {
			printf("Commande LED détectée : %s (envoie au microcontrôleur)\n", commande);
			commande_reconnaissance = true;
		}
		if (strcmp(commande, CHENILLARD_ON_1) == 0 || strcmp(commande, CHENILLARD_OFF_1) == 0 || strcmp(commande, CHENILLARD_ON_2) == 0 || strcmp(commande, CHENILLARD_OFF_2) == 0 || strcmp(commande, CHENILLARD_ON_3) == 0 || strcmp(commande, CHENILLARD_OFF_3) == 0 || strcmp(commande, CHENILLARD_FREQ_1) == 0 || strcmp(commande, CHENILLARD_FREQ_2) == 0 || strcmp(commande, CHENILLARD_FREQ_3) == 0) {
			printf("Commande Chenillard détectée : %s (envoie au microcontrôleur)\n", commande);
			commande_reconnaissance = true;
                }
		if (!commande_reconnaissance) {
			printf("Erreur : Commande inconnue. Tapez 'help' pour afficher la liste des commandes.\n");
                }
	}
	return 0;
}
