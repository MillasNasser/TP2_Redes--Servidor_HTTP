#ifndef _SERVERS_H_
#define _SERVERS_H_

// Bibliotecas importantes
	#include "connection.h"

// Funções
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
