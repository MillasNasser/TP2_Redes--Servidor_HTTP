#include "connection.h"
	
/**Captura um erro, finaliza o progrma e o exibe
*# @description: Servidor responsável por tratar as 
*# 		requisições na forma de o servidor aceita uma
*# 		conexão e a adiciona à uma fila que será 
*# 		consumida por um número finito de threads
*  
*? @params:
**   port: Porta a ser escutada pelo server
* 
*! @return:  */
void srv_iterativo(int port){
	char *resp;

	int sockfd = new_socket();
	struct sockaddr_in server;
	// Associou o socket a porta
	bind_socket(sockfd, port, &server);

	listen(sockfd, 10);
	while(1){
		clientent client;
		sckt_accept(sockfd, &client);

		/* Chamada da função que faz o tratamento da resposta */

		del_socket(client.sockfd); 
	}

	del_socket(sockfd);
}

/**Captura um erro, finaliza o progrma e o exibe
*# @description: Servidor responsável por tratar as 
*# 		requisições na forma sequêncial, onde o
*# 		servidor aceita a conexão e ele próprio responde
* 
*? @params:
**   port: Porta a ser escutada pelo server
*  
*! @return:  */
void srv_thread(int port){
	
}

/**Captura um erro, finaliza o progrma e o exibe
*# @description: Servidor responsável por tratar as 
*# 		requisições na forma de o servidor aceita uma
*# 		conexão e cria uma thread para responder
* 
*? @params:
**   port: Porta a ser escutada pelo server
*  
*! @return:  */
void srv_select(int port){
	
}

/**Captura um erro, finaliza o progrma e o exibe
*# @description: Servidor responsável por tratar as 
*# 		requisições na forma de o servidor acieta a
*# 		conexão e ele mesmo a trata, com o diferencial
*# 		de usar uma API que evita o read ser bloqueante
* 
*? @params:
**   port: Porta a ser escutada pelo server
*  
*! @return:  */
void srv_fila_task(int port){
	char *resp;

	int sockfd = new_socket();
	struct sockaddr_in server;
	// Associou o socket a porta
	bind_socket(sockfd, port, &server);

	listen(sockfd, 10);
	while(1){
		clientent client;
		sckt_accept(sockfd, &client);

		/* Chamada da função que faz o tratamento da resposta */

		del_socket(client.sockfd); 
	}

	del_socket(sockfd);
}