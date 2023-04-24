#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int serverSocket, newSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // Cria o socket
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Configura o endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(21);
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    // Associa o socket ao endereço do servidor
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof serverAddr);

    // Começa a escutar por conexões
    if (listen(serverSocket, 5) == 0) {
        cout << "Servidor FTP iniciado" << endl;
    } else {
        cout << "Erro ao iniciar o servidor FTP" << endl;
    }

    // Espera por conexões
    addr_size = sizeof clientAddr;
    newSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addr_size);

    // Envia a mensagem de boas-vindas para o cliente
    send(newSocket, "220 Welcome to my FTP server\r\n", strlen("220 Welcome to my FTP server\r\n"), 0);

    // Espera por comandos do cliente
    while (true) {
        memset(buffer, 0, sizeof buffer);
        recv(newSocket, buffer, 1024, 0);
        cout << buffer;

        if (strncmp(buffer, "USER", 4) == 0) {
            // Processa o comando USER
            send(newSocket, "331 Password required for user\r\n", strlen("331 Password required for user\r\n"), 0);
        } else if (strncmp(buffer, "PASS", 4) == 0) {
            // Processa o comando PASS
            send(newSocket, "230 User logged in\r\n", strlen("230 User logged in\r\n"), 0);
        } else if (strncmp(buffer, "QUIT", 4) == 0) {
            // Encerra a conexão com o cliente
            send(newSocket, "221 Goodbye\r\n", strlen("221 Goodbye\r\n"), 0);
            close(newSocket);
            break;
        } else {
            // Comando desconhecido
            send(newSocket, "500 Unknown command\r\n", strlen("500 Unknown command\r\n"), 0);
        }
    }

    // Encerra o socket do servidor
    close(serverSocket);

    return 0;
}
