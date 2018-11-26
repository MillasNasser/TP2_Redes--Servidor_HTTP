#include "servers.h"

/**Argumento 1 indica a opção de execução: 
 *     - 0 é servidor
 *     - 1 é cliente
 *
 * Argumento 2 recebe a porta de conexão 
 * 
 * Argumento 3 recebe o tipo de servidor a ser usado */
int main(int argc, char *argv[]) {
	if(argc != 3){ 
		printf("Execucao e feita por:"
			"./main.out <TIPO SERVER> <PORTA>\n"
			"<TIPO SERVER> pode ser:\n"
			"\t0: Servidor sequencial\n"
			"\t1: Servidor usando Threads\n"
			"\t2: Servidor usando Select\n"
			"\t3: Servidor usando Fila de Requisicoes\n"
			"<PORTA> pode ser qualquer valor entre 0 e "
			"65535.\nCaso houver um erro, mude o valor da porta\n"
		); exit(-1);
	}

	int opcao = atoi(argv[1]);
	int porta = atoi(argv[2]);

	switch(opcao) {
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
