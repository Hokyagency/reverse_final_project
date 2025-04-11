#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <stdbool.h>


//Definition des commandes
#define CMD_QUIT "quit"
#define CMD_HELP "help"
#define CMD_CLEAR "clear"


//-------------------------------------------------------------------------------------- FONCTIONS

int setup_serial_port(const char *port_name) {
    int fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("Unable to open serial port");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    options.c_iflag = IGNBRK;
    cfsetispeed(&options, B1152000);
    cfsetospeed(&options, B1152000);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

int write_serial_port(int fd, const char *data, size_t size) {
    return write(fd, data, size);
}

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

int main(int argc, char **argv) {
  	int ret = -1;
    int len = 0;
    int bytes_read = 0;
    struct pollfd fds;
    fds.events = POLLIN;


    if (argc < 2) {
        fprintf(stderr, "Usage: %s <serial_port>\n", argv[0]);
        return 1;
    }

    fds.fd = setup_serial_port(argv[1]);
    if (fds.fd == -1) {
        return 1;
    }

    char buffer[256];
    char buffer_sending[256];
    uint8_t ch;
	char commande[100];

	printf("Bienvenue dans le centre de contrôle du STM32 !\n");
	liste_cmd();

//--------------------------------------------------------------------------------------------- WHILE

	while (1) {
		printf(">");
        ret = poll(&fds, 1, 1000); // 1-second timeout
        if (ret > 0) 
        {
            if (fds.revents & POLLIN) {
                if(ioctl(fds.fd, FIONREAD, &bytes_read) == -1) {
                    printf("Error getting bytes available");
                    continue;;
                }
                printf("Bytes available: %d\n\r", bytes_read);
                len = read(fds.fd, buffer, bytes_read);

                if (len > 0) {
                    buffer[len] = '\0'; // Null-terminate the string
                    printf("Received: %s %d\r\n", buffer, len);
                    memset(buffer, 0, sizeof(buffer)); // Clear buffer
                    len = 0; // Reset length for next read
                } else if (len == 0) {
                    printf("No data received\n\r");
                    len = 0; // Reset length for next read
                    continue;
                } else {
                    printf("Error reading from serial port\n\r");
                    len = 0; // Reset length for next read
                    continue;
                }
            }
        }else if (ret == 0) {
            len = 0; // Reset length for next read
        }

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
<<<<<<< HEAD
                    			(strcmp(state, "ON") == 0 || strcmp(state, "OFF") == 0)) {
                    			printf("Commande LED reconnue : LED %d %s (envoi au microcontrôleur)\n", led_number, state);
					if(write_serial_port(fds.fd, commande, strlen(commande)) !=-1) {
						printf("Commande LED envoyée.\n");
=======
                    		(strcmp(state, "ON") == 0 || strcmp(state, "OFF") == 0)) {
                    		printf("Commande LED reconnue : LED %d %s (envoi au microcontrôleur)\n", led_number, state);
							if(write_serial_port(fds.fd, commande, strlen(commande)) !=-1) {
								printf("Commande LED envoyée.\n");
>>>>>>> d83c77fbd4023e411f6643525ff3cc96b4b42aa5
					}
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
<<<<<<< HEAD
                                        if(write_serial_port(fds.fd, commande, strlen(commande)) !=-1) {
                                                printf("Commande CHENILLARD envoyée.\n");
                                        }

=======
                        if(write_serial_port(fds.fd, commande, strlen(commande)) !=-1) {
                            printf("Commande CHENILLARD envoyée.\n");
                        }
>>>>>>> d83c77fbd4023e411f6643525ff3cc96b4b42aa5
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
<<<<<<< HEAD
                                        if(write_serial_port(fds.fd, commande, strlen(commande)) !=-1) {
                                                printf("Commande FREQUENCE envoyée.\n");
                                        }

=======
					if(write_serial_port(fds.fd, commande, strlen(commande)) !=-1) {
                            printf("Commande CHENILLARD envoyée.\n");
                        }
>>>>>>> d83c77fbd4023e411f6643525ff3cc96b4b42aa5
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
	close(fds.fd);
	return 0;
}
