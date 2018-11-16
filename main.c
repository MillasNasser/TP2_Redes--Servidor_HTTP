#include "servers.h"
#include "servers.h"

#define DIR "./sites"

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

void func_resp(int sockfd){
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

	} else {
		// Se a requisição for do tipo POST
		
	}
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
	
	while(1) {
		sckt_accept(sockfd, &client);

		func_resp(client.sockfd);
	}

	del_socket(client.sockfd);
	del_socket(sockfd);

    return 0;
}
