#ifndef _SERVERS_H_
#define _SERVERS_H_

// Bibliotecas importantes
	#include "connection.h"
	#include <pthread.h>

// Macros
	#define DIR "./sites"

// Funções
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
	int get_bytes(char *dir, char **bytes);

	/**Captura um erro, finaliza o progrma e o exibe
	*# @description: Função para tratamento direto 
	*# 			das requisições http.
	*  
	*? @params:
	**   sockfd: Descritor de arquivos vinculado ao socket
			do cliente.
	* 
	*! @return:  */
	void func_resp(int sockfd);

	/**Captura um erro, finaliza o progrma e o exibe
	*# @description: Função a ser executada pelas
	*# 			threads para atender as requisições
	*# 			http feitas pelo cliente.
	*  
	*? @params:
	**   client_void: Informações sobre o socket do cliente.
	* 
	*! @return:  */
	void *func_resp_thread(void *sockfd);

	/**Captura um erro, finaliza o progrma e o exibe
	*# @description: Servidor responsável por tratar as 
	*# 		requisições na forma sequêncial, onde o
	*# 		servidor aceita a conexão e ele próprio responde
	* 
	*? @params:
	**   port: Porta a ser escutada pelo server
	*  
	*! @return:  */
	void srv_thread(int port);

	/**Captura um erro, finaliza o progrma e o exibe
	*# @description: Servidor responsável por tratar as 
	*# 		requisições na forma de o servidor aceita uma
	*# 		conexão e cria uma thread para responder
	* 
	*? @params:
	**   port: Porta a ser escutada pelo server
	*  
	*! @return:  */
	void srv_select(int port);
	
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
	void srv_iterativo(int port);
	
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
	void srv_fila_task(int port);

#endif // _SERVERS_H_
