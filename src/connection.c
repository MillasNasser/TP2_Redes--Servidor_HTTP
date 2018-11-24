#include "connection.h"

/**
 * Captura um erro, finaliza o progrma e o exibe
 *? @params:
 **   FMT: Mensagem a ser exibida em caso de erro
 **   saida: número de erro capturado
 *  
 *! @return:  */
void sckt_error(const char *FMT, int saida){
    perror(FMT);
    exit(saida);
}

/**
 * Cria um socket
 *? @params:
 *  
 *! @return: É retornado um int simbolizando o socket */
int new_socket(){
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Se não foi possível criar
	if(sockfd == -1){
		sckt_error("<Connect::new_socket()> "\
					"Erro ao criar socket: ", errno);
	}

	int on = 1;
	if( -1 == setsockopt(
		sockfd, 
		SOL_SOCKET, 
		SO_REUSEADDR, 
		&on, 
		sizeof(int))
	){
		sckt_error("<Connect::new_socket()> "\
					"Erro ao criar socket: ", errno);
	}

	return sockfd;
}

/**
 * Deleta um socket
 *? @params:
 **   sockfd: Socket a ser deletado
 *  
 *! @return:  */
void del_socket(int sockfd){
	int error = close(sockfd);

	// Se não foi possível deletar
	// error != 0
	if(error){
		sckt_error("<Connect::del_socket()> "\
				"Erro ao criar socket: ", errno);
	}
}

/**
 * Pega informações de um host
 *? @params:
 **   hostname: Nome do host a fazer a consulta DNS
 *  
 *! @return: É retornado uma estrutura com o endereço e 
 *! outras informações do host */
struct hostent* new_host(char *hostName){
	struct hostent *host;
	host = gethostbyname(hostName);
	if(host == NULL){
		sckt_error("<Connect::new_host()> "\
				"Erro ao conseguir host: ", errno);
	}
	return host;
}


/**
 * Associa uma porta e um servidor ao socket
 *? @params:
 **   sockfd: Socket a ser associado
 **   port: Porta a ser associada
 **   server: Estruturas do server a ser associada 
 ** 		  junto com a porta
 *  
 *! @return:  */
void bind_socket(
		int sockfd, int port,
		struct sockaddr_in *server
	){
	
	/* Zerando a estrutura do endereço
	 * do servidor para escutar uma porta*/
	memset(server, 0, sizeof(struct sockaddr_in));
	
	/// Configurando as estruturas do server
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = INADDR_ANY;
	server->sin_port = htons(port);

	// Realizando a associação entre o socket e a porta
	int error = bind(sockfd,
					(struct sockaddr*)server, 
					sizeof(*server));

	if(error < 0){
		sckt_error("<Connect::bind_socket> "\
			"Erro ao associar socket a porta", errno);
	}
}

/* ------------------------------------------------------ */

/**
 * Faz o pedido de conexão a um server
 *? @params:
 **   sockfd: O tamanho do dado a ser enviado
 **   host: Se conectar, recebe as informações do servidor
 **   port: Porta recebendo a conexão
 *  
 *! @return: É retornado quantos bytes foram enviados */
void sckt_connect(int sockfd, struct hostent *host, int port){
	struct sockaddr_in serv_addr;
	
	// Zerando a estrutura geral do server
	memset(&serv_addr,0,sizeof(serv_addr));
	
	// Definindo o protocolo IPv4
	serv_addr.sin_family = AF_INET;
	
	// Copiando o endereço do host para acessa-lo
	memcpy(
		&(serv_addr.sin_addr.s_addr), 
		host->h_addr_list[0], 
		host->h_length
	);

	serv_addr.sin_port = htons(port);
	
	// Conectando ao host
	int erro = connect(
		sockfd,
		(struct sockaddr *) &serv_addr,
		sizeof(serv_addr)
	);

	if(erro < 0){
		sckt_error("<Connect::sckt_connect> "\
			"Erro ao conectar ao host", errno);
	}
}

/**
 * Aceita uma conexão de um cliente
 *? @params:
 **   sockfd: Socket a ser enviado o dado
 **   client: Se conectar, recebe as informações do cliente
 *  
 *! @return: É retornado quantos bytes foram enviados */
void sckt_accept(int sockfd, clientent *client){
	client->socklen = sizeof(client->sock_addr);
	client->sockfd = accept(
		sockfd, 
		(struct sockaddr*)&(client->sock_addr),
		&(client->socklen)
	);

	if(client->sockfd < 0){
		sckt_error("<Connect::sckt_connect> "\
			"Erro ao aceitar a conexao", errno);
	}
}

/**
 * Envia dados pelo socket
 *? @params:
 **   socket: Socket a ser enviado o dado
 **   buffer: O dado a ser enviado
 **   bufSize: O tamanho do dado a ser enviado
 *  
 *! @return: É retornado quantos bytes foram enviados */
int sckt_send(int sockfd, void *buffer, size_t bufSize){
	int erro = 0;
	int bytes_writen = 0;
	
	if(write(sockfd, &bufSize, sizeof(size_t)) < 0){
		sckt_error("<Connect::sckt_send> "\
				"Erro ao enviar tamanho do dado", errno);
	}

	while(bytes_writen < bufSize){
		erro = write(
			sockfd, 
			buffer + bytes_writen, 
			bufSize - bytes_writen
		);
		
		if(erro < 0){
			sckt_error("<Connect::sckt_send> "\
				"Erro ao enviar dados", errno);
		}else{
			bytes_writen += erro;
		}
	}
	
	return bytes_writen;
}

/**
 * Recebe dados pelo socket
 *? @params:
 **   socket: Socket a receber o dado
 **   bufSize: O tamanho do dado a ser recebido
 *
 *! @return: É retornado um vetor com o conteúdo lido 
 *
 *?obs: No parâmetro bufSize é preciso só passar o tamanho
 *?		do buffer,pois sempre adiciona 1 byte a mais para
 *?		evitar overflow na memória */
char* sckt_recv(int sockfd){
	// Recebendo o tamanho do buffer a ser enviado
	size_t bufSize;
	if(read(sockfd, &bufSize, sizeof(size_t)) < 0){
		sckt_error("<Connect::sckt_recv> "\
				"Tamanhho nao identificado", errno);
	}
	
	// Criando o buffer
	char *buffer = malloc(bufSize);

	// Variaveis que irão gerenciar o loop
	int erro = 0;
	int bytes_read = 0;

	while(bytes_read < bufSize){
		erro = read(
			sockfd, 
			buffer + bytes_read, 
			bufSize - bytes_read
		);

		if(erro < 0){
			sckt_error("<Connect::sckt_recv> "\
				"Erro ao receber dados", errno);
		}else{
			bytes_read += erro;
		}
	}

	return buffer;
}

/**
 * Recebe dados pelo socket
 *? @params:
 **   socket: Socket a receber o dado
 **   size: O tamanho do dado a ser recebido
 *
 *! @return: É retornado um vetor com o conteúdo lido */
char* sckt_recvn(int sockfd, size_t size){
	// Criando o buffer
	char *buffer = malloc(size);

	// Variaveis que irão gerenciar o loop
	int erro = 0;
	int bytes_read = 0;
	int count = 0;

	while(bytes_read < size && count != 1000){
		erro = read(
			sockfd, 
			buffer + bytes_read, 
			size - bytes_read
		);

		if(erro < 0){
			sckt_error("<Connect::sckt_recv> "
				"Erro ao receber dados", errno);
		}else{
			bytes_read += erro;
			count = 0;
		}

		printf("erro: %d\n", bytes_read);

		count++;
	}

	return buffer;
}

/* Tratamento específico para o HTTP */
int sckt_http_send(int sockfd, void *buffer, size_t bufTam){
	int erro = 0;
	int bytes_writen = 0;
	size_t bufSize = bufTam;

	while(bytes_writen < bufSize){
		erro = write(
			sockfd, 
			buffer + bytes_writen, 
			bufSize - bytes_writen
		);
		
		if(erro < 0){
			sckt_error("<Connect::sckt_http_send> "\
				"Erro ao enviar dados", errno);
		}else{
			bytes_writen += erro;
		}
	}
	
	return bytes_writen;
}

/**
 * Recebe dados pelo socket
 *? @params:
 **   socket: Socket a receber o dado
 **   size: O tamanho do dado a ser recebido
 *
 *! @return: É retornado um vetor com o conteúdo lido */
char* sckt_http_recv(int sockfd, size_t bufSize){
	// Criando o buffer
	char *buffer = malloc(bufSize);

	// Variaveis que irão gerenciar o loop
	int erro = 0;
	int bytes_read = 0;

	while(bytes_read < bufSize){
		erro = read(sockfd, buffer + bytes_read, PACK_SIZE);

		if(erro < 0){
			sckt_error("<Connect::sckt_http_recv> "
				"Erro ao receber dados", errno);
		}else{
			bytes_read += erro;
		}

		if(buffer[bytes_read-1]=='\n' && buffer[bytes_read-2]=='\r'){
			break;
		}
	}

	return buffer;
}