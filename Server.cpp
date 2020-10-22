#include "stdafx.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <fstream>

using namespace std;

sigset_t blocked_orig;
sigset_t blocked_new;
int ret;

int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    while (1)
    {
        sock = accept(listener, NULL, NULL);
        if (sock < 0)
        {
            perror("accept");
            exit(3);
        }

        while (1)
        {
            ret = sigemptyset(&blocked_new);
            ret = sigaddset(&blocked_new, SIGHUP);
            ret = sigaddset(&blocked_new, SIGTERM);

            ret = sigprocmask(SIG_BLOCK, &blocked_new, &blocked_orig);
            if (ret == -1) {
                perror("sigprocmask()");
                exit(1);
            }
            ofstream fout;

            fout.open("Server.txt", ios_base::out | ios_base::app);
            char buf[sizeof(fout)];
            memset(buf, 0, sizeof(fout));
            if (!fout.is_open())
            {
                perror("Could not open Server.txt");
                fout.clear();
            }
            recv(sock, buf, sizeof(buf), 0);
            fout << buf;
            fout.close();

            printf("Client connected\n");

            ret = sigprocmask(SIG_UNBLOCK, &blocked_new, NULL);
            if (ret == -1) {
                perror("sigprocmask()");
                exit(1);
            }
        }

        close(sock);
    }

    return 0;
}
