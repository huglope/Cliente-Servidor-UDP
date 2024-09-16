// Practica 5: López Álvarez, Hugo
// @author Hugo López Álvarez
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){

	// Se declaran las variables que se van a utilizar
	int puerto, sock, num_letra, longitud; 
	socklen_t tamcliente;
	char letra, recibido[80], *respuesta;
	struct sockaddr_in dirsock, dircliente;

	// Se comprueba que el numero de argumentos sea correcto
	if(argc !=1 && argc!=3 ){
		fprintf(stderr, "La estructura que debe tener es : ./echocon-udp-server-apellidos [-p puerto-servidor]");
		exit(-1);
	}

	// En caso de que haya 3 argumentos, se comprueba que la flag introducida sea -p
	if(argc == 3 && strcmp(argv[1], "-p") != 0){
		fprintf(stderr, "La estructura que debe tener es : ./echocon-udp-server-apellidos [-p puerto-servidor]");
                exit(-1);
        }

	// Se asignan los puertos segun lo que haya introducido el usuario
	if (argc == 1) puerto = 5;
	else puerto = atoi(argv[2]);

	// Se abre un socket para la comunicación
        if((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0){
                perror("socket()");
                exit(EXIT_FAILURE);
        }

	// Se guarda la informacion pertinente en la estructura
	dirsock.sin_family = AF_INET;
	dirsock.sin_port = htons(puerto);	
	dirsock.sin_addr.s_addr = INADDR_ANY;
	
	// Se enlaza un socket con una “dirección” (IP, puerto y protocolo) local
	if(bind(sock, (struct sockaddr *) &dirsock, sizeof(struct sockaddr_in)) < 0){
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	// Empieza el bucle en el que se responderan las solicitudes
	while(1){
		tamcliente = sizeof(struct sockaddr);

		// Se recibe la cadena a transformar
		if((longitud =recvfrom(sock, recibido, 80, 0,(struct sockaddr *) &dircliente, &tamcliente)) < 0){
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}
		printf("mensaje recibido de IP=%s\n", inet_ntoa(dircliente.sin_addr));
		
		// Se asigna un tamaño al atributo repuesta
		if ((respuesta = (char*) malloc (strlen(recibido)+1)) == NULL){
			perror("malloc()");
			exit(EXIT_FAILURE);
		}

		// Se transforma la cadena
		for(num_letra = 0; num_letra < strlen(recibido); num_letra++){
			letra = recibido[num_letra];
			if(letra >= 'A' && letra <= 'Z') letra =  letra + ('a' - 'A');
			else if (letra >= 'a' && letra <= 'z') letra = letra - ('a' - 'A');
			respuesta[num_letra] = letra;
		}		

		respuesta [num_letra] = '\0';

		// Se envia la cadena ya transformada
		if(sendto(sock, respuesta, longitud, 0,(struct sockaddr *) &dircliente, tamcliente) < 0){
			perror("sendto()");
			exit(EXIT_FAILURE);
		}

		// Se libera la memoria y los atributos se reestablecen
		free(respuesta);
		strcpy(recibido, "");
	}
	
	// En un caso hipotético se cerraria el socket, pero en este caso, nunca se alcanzara esta parte del codigo
	close(sock);
	return 0;

}
