#include "servers.h"

/**Captura um erro.
*# @description: Função destinada a leitura dos bytes
		do arquivo a ser enviado para o cliente.
*  
*? @params:
**   dir: Diretório do arquivo.
	 bytes: ponteiro onde será guardada
		a referência para o vetor de bytes
		correspondentes ao arquivo.
* 
*! @return:  */
int get_bytes(char *dir, char **bytes) {
	// Retorna o número de bytes lidos e atribui a referência do vetor
	// que contém os dados do arquivo lido no ponteiro *bytes do subprograma pai

	FILE *f = fopen(dir, "r");
	if(!f) {
		// Se houverem problemas na abertura do arquivo

		// *bytes é NULL e o número de bytes lidos sendo retornado é zero
		*bytes = NULL;
		return 0;
	}

	// Leitura e contagem dos bytes do documento a ser lido
	fseek(f, 0, SEEK_END);
	int num_bytes = ftell(f);
	rewind(f);
	*bytes = malloc(num_bytes);
	fread(*bytes, num_bytes, 1, f);
	fclose(f);
	return num_bytes;
}

/**Captura um erro, finaliza o progrma e o exibe
*# @description: Função para tratamento direto 
*# 			das requisições http.
*  
*? @params:
**   sockfd: Descritor de arquivos vinculado ao socket
		do cliente.
* 
*! @return:  */
void func_resp(int sockfd) {
	char *resp = sckt_http_recv(sockfd, BUFF_SIZE),  
		*bytes, *http_doc,
		dir[256],head[256],
		tipo[16], argumento[256], versao[16];
	
	printf("%s\n", resp);
	sscanf(resp, "%s %s %s", tipo, argumento, versao);
	
	if(!strcmp(tipo, "GET")) {
		// Se a requisição for do tipo GET
		// Montar o caminho do arquivo solicitado como argumento
		sprintf(dir, "%s%s", DIR, argumento);
		if(!strcmp(argumento, "/")) {
			strcat(dir, "index.html");
		}

		// Ler os bytes do arquivo requisitado
		int nbytes = get_bytes(dir, &bytes);
		int bufTam;

		if(!bytes) {
			// Se o vetor de bytes para envio no corpo do doc HTTP estiver nulo

			// Copiar a mensagem de erro para o buffer http_doc
			char erro_404[] = "HTTP/1.0 404 Not Found\r\n\r\n";
			http_doc = malloc(sizeof(erro_404));
			memcpy(http_doc, erro_404, sizeof(erro_404));

			// Determinar tamanho do buffer a ser enviado
			bufTam = strlen(http_doc);
		} else {
			// Se bytes conter os dados do arquivo
			// Montar o buffer http_doc com o cabeçalho e o corpo

			// Montando cabeçalho
			sprintf(head, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n", nbytes);
			int tam_head = strlen(head);

			// Determina o tamanho do buffer que será enviado
			bufTam = tam_head + 1 + nbytes;

			// Copiando o cabeçalho e os bytes do documento a ser enviado para o buffer http_doc
			http_doc = malloc(bufTam);
			sprintf(http_doc, "%s", head);
			memcpy(http_doc + tam_head, bytes, nbytes);
			free(bytes);
		}

		// Mensagem de resposta montada
		// Enviando mensagem de resposta para o cliente
		sckt_http_send(sockfd, http_doc, bufTam);

		free(http_doc);	
		free(resp);

	}
}

/**Captura um erro, finaliza o progrma e o exibe
*# @description: Função a ser executada pelas
*# 			threads para atender as requisições
*# 			http feitas pelo cliente.
*  
*? @params:
**   client_void: Informações sobre o socket do cliente.
* 
*! @return:  */
void *func_resp_thread(void *client_void) {
	clientent *client = ((clientent *) client_void);
	func_resp(client->sockfd);

	//Deletando socket do cliente e encerrando a conexão
	del_socket(client->sockfd);

	free(client);
	return NULL;
}

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

	int sockfd = new_socket();
	struct sockaddr_in server;
	pthread_t tid;

	bind_socket(sockfd, port, &server);
	listen(sockfd, 712);
	while(1) {
		clientent *client = malloc(sizeof(clientent));
		sckt_accept(sockfd, client);
		
		pthread_create(&tid, NULL, func_resp_thread, (void *) client); 
    	pthread_join(tid, NULL);
	}

	del_socket(sockfd);
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
	int sockfd = new_socket();
	struct sockaddr_in server;
	pthread_t tid;

	bind_socket(sockfd, port, &server);
	listen(sockfd, 712);

	int maior_fd = sockfd;
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);

	while(1) {
		int nfds;
		nfds = select(maior_fd + 1, &fdset, NULL, NULL, NULL);

		if(nfds == -1) {
			sckt_error("<Servers::srv_select> "\
			"erro ao executar select()", errno);
		} else {
			clientent client;
			for(int i = 0; i <= maior_fd; i++) {
				if(i == sockfd) {
					sckt_accept(sockfd, &client);
					FD_SET(client.sockfd, &fdset);
					if(maior_fd < client.sockfd) {
						maior_fd = client.sockfd;	
					}
				} else {
					if(FD_ISSET(i, &fdset)) {
						func_resp(i);
						FD_CLR(i, &fdset);
						del_socket(i);
					}
				}
			}
		}
	}

	del_socket(sockfd);
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