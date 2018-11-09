#ifndef _CONNECT_H_
#define _CONNECT_H_
// Inclusão da biblioteca padrão
    #include "header_padrao.h"

// Definições importantes
    #define BUFF_SIZE 2048

    // Estrutura de retorno quando se aceita uma
    // conexão com usuários
    typedef struct{
        int sockfd;
        struct sockaddr_in sock_addr;
        socklen_t socklen;
    }clientent;

// Funções

    // Funções de envio e recebimento
    /** Envia dados pelo socket
    *? @params:
    **   socket: Socket a ser enviado o dado
    **   buffer: O dado a ser enviado
    **   bufSize: O tamanho do dado a ser enviado
    *  
    *! @return: É retornado quantos bytes foram enviados */
    int sckt_send(int sockfd, void *buffer, size_t bufSize);
    
    /** Recebe dados pelo socket
    *? @params:
    **   socket: Socket a receber o dado
    *
    *! @return: É retornado um vetor com o conteúdo lido */
    char* sckt_recv(int sockfd);

    /** Recebe n bytes de dados pelo socket
    *? @params:
    **   socket: Socket a receber o dado
    **   bufSize: O tamanho do dado a ser recebido
    *
    *! @return: É retornado um vetor com o conteúdo lido */
    char* sckt_recvn(int sockfd, size_t size);

    // ********* Manipulação de sockets ********* //
    /** Cria um socket
    *? @params:
    *  
    *! @return: É retornado um int simbolizando o socket */
    int new_socket();

    /** Deleta um socket
    *? @params:
    **   sockfd: Socket a ser deletado
    *  
    *! @return:  */
    void del_socket(int sockfd);

    // ********* Manipulação de hosts ********* //
    /** Pega informações de um host
    *? @params:
    **   hostname: Nome do host a fazer a consulta DNS
    *  
    *! @return: É retornado uma estrutura com o endereço e 
    *! outras informações do host */
    struct hostent* new_host(char * hostName);

    /** Associa uma porta e um servidor ao socket
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
    );

    // ******** Gerencia de conexões dos sockets ******** //
    /** Faz o pedido de conexão a um server
    *? @params:
    **   sockfd: O tamanho do dado a ser enviado
    **   host: Se conectar, recebe as informações do servidor
    **   port: Porta recebendo a conexão
    *  
    *! @return: É retornado quantos bytes foram enviados */
    void sckt_connect(int sockfd, struct hostent *host, int port);
    
    /** Aceita uma conexão de um cliente
    *? @params:
    **   sockfd: Socket a ser enviado o dado
    **   client: Se conectar, recebe as informações do cliente
    *  
    *! @return: É retornado quantos bytes foram enviados */
    void sckt_accept(int sockfd, clientent *client);
    
    //! Lançamento de erros
    /** Captura um erro, finaliza o progrma e o exibe
    *? @params:
    **   FMT: Mensagem a ser exibida em caso de erro
    **   saida: número de erro capturado
    *  
    *! @return:  */
    void sckt_error(const char *FMT, int saida);
#endif //_CONNECT_H_