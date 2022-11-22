#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

// socket
#include <sys/types.h>          
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

// socket
#include <sys/types.h>          
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 21
#define MAX_SIZE 256
#define LOGIN_MAX 64
#define SENHA_MAX 32

char *host;

void telaInicial() {
    printf("\n\n");
    printf("--------------------------------------------------------------------\n");
    printf("                                                                    \n");
    printf("    Trabalho: Interação Cliente/Servidor FTP com resolução DNS      \n");
    printf("                                                                    \n");
    printf("    Disciplina: Redes de Computadores       Prof.: João Borges      \n");
    printf("                                                                    \n");
    printf("    Equipe: Raquel Lima                                             \n");
    printf("            Wanessa                                                 \n");
    printf("                                                                    \n");
    printf("-------------------------------------------------------------------");
}

char respostaSocket(int sock, char *buffer) 
{
    int nbytes;
    if ((nbytes = recv(sock, buffer, MAX_SIZE, 0)) == -1)
    {
        fprintf(stderr, "Dados não recebidos.\n");
        exit(EXIT_FAILURE);
    }
    buffer[nbytes]='\0';
    printf("%s", buffer);
}

int login(int sock, char *buffer) 
{
  	int tentativas = 0;
	char login[LOGIN_MAX];
    char senha[SENHA_MAX];
    char logname[LOGIN_MAX] = "demo";
    char passname[LOGIN_MAX] = "password";
	
	printf("\nInforme o Login: ");
    scanf("%s", login);
    strcpy(login, "USER ");
    strcat(login, logname);
    strcat(login, "\r\n");
    strcpy(buffer, login);
    send(sock, buffer, strlen(buffer), 0);
    respostaSocket(sock, buffer);
	
  do 
  { 
    printf("\nInforme a Senha: ");
    scanf("%s", senha);
	  
    if (strcmp(login, logname) && !strcmp(senha, passname)) 
    {
        strcpy(senha, "PASS ");
        strcat(senha, passname);
        strcat(senha, "\r\n");
        strcpy(buffer, senha);
        send(sock, buffer, strlen(buffer), 0);
        respostaSocket(sock, buffer);
        printf("- Login realisado com sucesso.\n");
        return 1;
    } 
    else 
    {
        printf("- Senha invalida!\n");
        tentativas++;
    }
    } while (tentativas < 5);
    printf("\nNumero maximo de tentativas alcançado!\n");
    return 0;
}

int passivePort(char *buffer) 
{
    char comando[MAX_SIZE];
    strcpy(comando, buffer);

    char mat[100][100];
    char *ptr;
    int i = 0;
    int p1, p2, port;

    ptr = strtok(comando, "(), .");
    while (ptr) {
        strcpy(mat[i], ptr);
        ptr = strtok(NULL, "(), .");
        i++;
    } 
    p1 = atoi(mat[8])*256;
    p2 = atoi(mat[9]);
    port = p1 + p2;

    return port;
}

// função disponível em https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
char** str_split(char* buffer, char delim, int* num)
{
    char** ret;
    int retLen;
    char* c;

    if ((buffer == NULL) || (delim == '\0'))
    {
        ret = NULL;
        retLen = -1;
    }
    else
    {
        retLen = 0;
        c = buffer;
        do
        {
            if (*c == delim)
            {
                retLen++;
            }

            c++;
        } while (*c != '\0');

        ret = malloc((retLen + 1) * sizeof(*ret));
        ret[retLen] = NULL;

        c = buffer;
        retLen = 1;
        ret[0] = buffer;

        do
        {
            if (*c == delim)
            {
                ret[retLen++] = &c[1];
                *c = '\0';
            }

            c++;
        } while (*c != '\0');
    }

    if (num != NULL)
    {
        *num = retLen;
    }

    return ret;
}

int main( int argc, char* argv[] )
{
    char buffer[MAX_SIZE];

    int sock, conn, num, len;
    char *addr;
    char** argumentos;

    struct sockaddr_in servaddr;
    struct hostent *host_info;
    
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <host>\n", argv[0]);
        exit(EXIT_FAILURE);
    } else 
        telaInicial();
        host = argv[1];

    host_info = gethostbyname(host);
	if (!host_info) {
		fprintf(stderr, "Host desconhecido.\n");
		exit(EXIT_FAILURE);
	} else 
        addr = inet_ntoa(*(struct in_addr *) (host_info->h_addr));

    // CRIA E INICIALIZA O SOCKET
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "Permissão para criar o socket negada.\n");
        exit(EXIT_FAILURE);
    }

    // ESTRUTURA DO SERVIDOR
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr(addr); 

    // FAZ A CONEXAO COM O SERVIDOR
    conn = connect(sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
    if (conn == -1) {
        fprintf(stderr, "A solicitação para a conexão falhou.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("\nConectado ao servidor %s (%s) na porta %d.\n", host, addr, PORT);
        respostaSocket(sock, buffer);
    }
    
    // COMANDOS FTP
    login(sock, buffer);

    while((strcmp(buffer, "quit") != 0) || (strcmp(buffer, "QUIT") != 0))
    {  
        printf("\nftp>> ");
        fgets(buffer, MAX_SIZE, stdin);

        char * enter;
        if((enter = strrchr(buffer, '\n')) != NULL){
            *enter = '\0';
        }

        argumentos = str_split(buffer, ' ', &num);

        if ((strcmp(buffer, "pasv") == 0) || (strcmp(buffer, "PASV") == 0)) 
        {
            send(sock, "PASV\r\n", strlen("PASV\r\n"), 0);
            respostaSocket(sock, buffer);
            int porta = passivePort(buffer);
            printf("Conecte-se à PORTA %d\n", porta);
        }
        else if ((strcmp(buffer, "list") == 0) || (strcmp(buffer, "LIST") == 0)) 
        {
            send(sock, "LIST\r\n", strlen("LIST\r\n"), 0);
            respostaSocket(sock, buffer);
        }
        else if ((strcmp(buffer, "retr") == 0) || (strcmp(buffer, "RETR") == 0))
        {
            if (num != 2)
            {
                printf("\nUso: retr <arquivo> ");
            } 
            else 
            { 
                char comando[50];
                strcpy(comando, "RETR ");
                strcat(comando, argumentos[1]);
                strcat(comando, "\r\n");
                strcpy(buffer, comando);
                send(sock, buffer, strlen(buffer), 0);
                respostaSocket(sock, buffer); 
            }
        }
        else if ((strcmp(buffer, "cwd") == 0) || (strcmp(buffer, "CWD") == 0))
        {
            if (num != 2)
            {
                printf("\nUso: cwd <diretorio> ");
            } 
            else 
            { 
                char comando[50];
                strcpy(comando, "CWD ");
                strcat(comando, argumentos[1]);
                strcat(comando, "\r\n");
                strcpy(buffer, comando);
                send(sock, buffer, strlen(buffer), 0);
                respostaSocket(sock, buffer);
                
            }
        }
        else if ((strcmp(buffer, "pwd") == 0) || (strcmp(buffer, "CWD") == 0)) 
        {
            send(sock, "PWD\r\n", strlen("PWD\r\n"), 0);
            respostaSocket(sock, buffer);
        }
        
    }
    // QUIT
    send(sock, "QUIT\r\n", strlen("QUIT\r\n"), 0);
    respostaSocket(sock, buffer);

    // ENCERRA O SOCKET
    close(sock);

    return 0;
}