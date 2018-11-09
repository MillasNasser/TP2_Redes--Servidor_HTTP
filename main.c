#include "connection.h"

void server(int port){
	char *resp;

	int sockfd = new_socket();
	struct sockaddr_in server;
	// Associou o socket a porta
	bind_socket(sockfd, port, &server);

	listen(sockfd, 10);
	//while(1){
		clientent client;
		sckt_accept(sockfd, &client);

		resp = sckt_recv(client.sockfd);
		printf("%s: %s\n", inet_ntoa(
			client.sock_addr.sin_addr),
			resp); 

		free(resp);

		sckt_send(client.sockfd, "KKK EAE MEN", sizeof("KKK EAE MEN")+1);
		del_socket(client.sockfd); //Não vai estar aqui :D
	//}

	del_socket(sockfd);
}

void client(char *serv, int port){
	char *resp; 
	int sockfd = new_socket();
	struct hostent *server = new_host(serv);

	sckt_connect(sockfd, server, port);
	printf("Cliente conectou\n");

	sckt_send(sockfd, "Olha aquui", sizeof("Olha aquui")+1);
	resp = sckt_recv(sockfd);
	printf("%s: %s\n",server->h_name, resp);
	free(resp);

	del_socket(sockfd);
}

/**Argumento 1 indica a opção de execução: 
 *     - 0 é servidor
 *     - 1 é cliente
 *
 * Argumento 2 recebe a porta de conexão */
int main(int argc, char *argv[]) {
	if(argc < 3){ 
		sckt_error("Nao ha parametros suficientes!", -1);
	}

	int porta = atoi(argv[2]);
	int opcao = atoi(argv[1]);

	int sockfd = new_socket();
	struct sockaddr_in server;

	bind_socket(sockfd, porta, &server);
	listen(sockfd, 10);

	clientent client;
	sckt_accept(sockfd, &client);

	char *resp = sckt_recvn(client.sockfd, BUFF_SIZE);
	printf("%s\n", resp); free(resp);

	del_socket(client.sockfd);
	del_socket(sockfd);
    return 0;
}


/* Coisa de testar depois */
/*
	switch(opcao){
		case 0: server(porta); break;
		case 1: client("127.0.0.1", porta); break;
	}
*/