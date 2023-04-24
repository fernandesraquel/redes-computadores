#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // Cria o socket
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Configura o endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(21);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    // Conecta ao servidor
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

    // Recebe a mensagem de boas-vindas do servidor
    recv(clientSocket, buffer, 1024, 0);
    cout << buffer << endl;

    // Envia um comando FTP para o servidor
    send(clientSocket, "USER username\r\n", strlen("USER username\r\n"), 0);
    recv(clientSocket, buffer, 1024, 0);
    cout << buffer << endl;

    // Envia outro comando FTP para o servidor
    send(clientSocket, "PASS password\r\n", strlen("PASS password\r\n"), 0);
    recv(clientSocket, buffer, 1024, 0);
    cout << buffer << endl;

    // Encerra a conexão com o servidor
    close(clientSocket);

    return 0;
}
