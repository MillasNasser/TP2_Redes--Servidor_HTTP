#include "servers.h"

/**Argumento 1 indica a opção de execução: 
 *     - 0 é servidor
 *     - 1 é cliente
 *
 * Argumento 2 recebe a porta de conexão 
 * 
 * Argumento 3 recebe o tipo de servidor a ser usado */
int main(int argc, char *argv[]) {
	if(argc != 4){ 
		sckt_error("Nao ha parametros suficientes!", -1);
	}

	int porta = atoi(argv[2]);
	int opcao = atoi(argv[1]);
	int serv_tipo = atoi(argv[3]);

	if(opcao != 0 && opcao != 1) {
		sckt_error("Parâmetros errados!", -1);
	}

	switch(serv_tipo) {
		case 0:
			srv_iterativo(porta);
		break;

		case 1:
			srv_thread(porta);
		break;

		case 2:
			srv_select(porta);
		break;

		case 3:
			srv_fila_task(porta);
		break;

		default:
			sckt_error("Parâmetros errados!", -1);
		break;
	}

    return 0;
}
