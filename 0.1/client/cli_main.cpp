#include "client.h"
#include "../socket/socket.h"

int main()
{
    int connfd = my_socket::create_socket_for_client();
    client cli(connfd);

    do
    {
        cli.get_cmd();

        if(  -1 == cli.dispatch() )
        {
            ;// Some error occur.
        }

    }while( cli.is_run() );

/*
    while( 1 )
    {
        char buff[BUFSIZ] = {0};

        fgets(buff, BUFSIZ, stdin);

        send(connfd, buff, strlen(buff)-1, 0);

        if(0 == strncmp(buff, "exit", 4))
        {
            break;
        }

        bzero(buff, strlen(buff));

        recv(connfd, buff, BUFSIZ, 0);

        cout << buff << endl;
    }
*/
    close(connfd);

    return 0;
}
