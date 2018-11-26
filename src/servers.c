#include "servers.h"

/**********************************************************/
/************ Tratamento do arquivo solicitado ************/
/**********************************************************/
/**Faz a leitura dos bytes de um arquivo para um vetor
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

/*********************************************************/
/************ Tratamento das requisições HTTP ************/
/*********************************************************/

/**Faz a resposta dos servidores para as requisições HTTP
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

/**Realiza a resposta para o server de threads
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

/*********************************************************/
/****************** Servidor Sequencial ******************/
/*********************************************************/
void srv_iterativo(int port){
	int sockfd = new_socket();
	struct sockaddr_in server;
	// Associou o socket a porta
	bind_socket(sockfd, port, &server);

	listen(sockfd, 10);
	while(1){
		clientent client;
		sckt_accept(sockfd, &client);

		func_resp(client.sockfd);

		del_socket(client.sockfd); 
	}

	del_socket(sockfd);
}

/*********************************************************/
/************** Servidor utilizando Threads **************/
/*********************************************************/
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

/*********************************************************/
/************** Servidor utilizado o Select **************/
/*********************************************************/
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

/**********************************************************/
/************ Servidor com Fila de Requisições ************/
/**********************************************************/
int row_start = 0, row_end = 0;
Fila_Socket fila_requisicao[REQUEST_NUM] = {0};

pthread_mutex_t fila_mutex;
pthread_cond_t fila_cond;

void* consumidor(void *arg){
	Fila_Socket client;
	int id_thread = *(int*) arg;
	while(1){
		// Se não houver requisições vá dormir
		if(row_end == row_start){
			pthread_cond_wait(&fila_cond, &fila_mutex);
		}

		// pega a primeira requisição da fila ao acordar
		if(pthread_mutex_trylock(&fila_mutex)){
			client = fila_requisicao[row_start];
			printf("Thread %d pegou a requisicao\n", id_thread);
			row_start++;
			row_start = row_start % REQUEST_NUM;
			pthread_mutex_unlock(&fila_mutex);
		}else{
			printf("Thread %d nao deu lock\n", id_thread);
			continue;
		}

		// Tratar a requisição ao pegá-la
		func_resp(client.sockfd);
		del_socket(client.sockfd); // removê-la da fila
		client.estado = 0; //Definir a posição como vazia
	}
}

void srv_fila_task(int port){
	int i;
	int sockfd = new_socket();
	struct sockaddr_in server;

	//memset(&fila_requisicao, 0, sizeof(Fila_Socket*)*REQUEST_NUM);
	pthread_mutex_init(&fila_mutex, NULL);
	pthread_cond_init(&fila_cond, NULL);

	pthread_t *nthreads[N_THREADS];
	for(i = 0; i < N_THREADS; i++){
		int j = i;
		printf("Disparando a thread %d\n", i);
		pthread_create(&(nthreads[i]), NULL, consumidor, &j);
	}

	// Associou o socket a porta
	bind_socket(sockfd, port, &server);
	listen(sockfd, 10);
	while(1){
		// O número deve ser igual ao maximo REQUEST_NUM-1
		if(row_end+1 == row_start) continue; //Mudar para wait

		clientent client;
		sckt_accept(sockfd, &client);

		// Capturando as requisições a serem tratadas
		pthread_mutex_lock(&fila_mutex);
			// Ainda não foi consumido o dado
			if(fila_requisicao[row_end].estado == 1){
				// Chamando outra thread para ajudar
				pthread_cond_signal(&fila_cond);
				pthread_mutex_unlock(&fila_mutex);
				continue;
			}

			// Adicionando a requisição à fila
			fila_requisicao[row_end].sockfd = client.sockfd;
			fila_requisicao[row_end].estado = 1;
			row_end++;
			row_end = row_end % REQUEST_NUM;

			// Enviando sinal a uma thread consumidora
			pthread_cond_signal(&fila_cond);
		pthread_mutex_unlock(&fila_mutex);
	}

	del_socket(sockfd);
}