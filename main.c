#include "servers.h"
#include "servers.h"

void func_resp(int sockfd){
	char *resp = sckt_http_recv(sockfd, BUFF_SIZE);
	printf("%s\n", resp); free(resp);

	sckt_http_send(sockfd, 
		"HTTP/1.0 200 OK\n"
		"\n"
		"<html>\n"
		"<head>\n"
		"<title> Como assim? </title>\n"
		"</head>\n"
		"<body>\n"
		"<h1> Olha como faz </h1>\n"
		"</body>\n"
		"</html>\r\n"
	);
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
	listen(sockfd, 712);

	clientent client;
	sckt_accept(sockfd, &client);

	func_resp(client.sockfd);

	del_socket(client.sockfd);
	del_socket(sockfd);

    return 0;
}
