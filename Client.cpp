#include "stdafx.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>

using namespace std;

int main()
{
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425); // или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

    ifstream fin;

    fin.open("Client.txt", ios_base::in | ios_base::app);
    static char buf[sizeof(fin)];
    memset(buf, 0, sizeof(fin));
    if (!fin.is_open())
    {
        perror("Could not open Client.txt");
        fin.clear();
    }
    while (fin.getline(buf, sizeof(fin)))
        send(sock, buf, sizeof(buf), 0);
    fin.clear();
    fin.close();

    printf("Connect\n");
    
    system("pause");
    close(sock);

    return 0;
}