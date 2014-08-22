#include "client.h"
#include "../socket/socket.h"

void welcome()
{
    printf("\033[1;1H\033[2J");

    int blankspace = 0;
    int newline = 0;
    struct winsize size = {0};
    if(0 == ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) )
    {
        int row = size.ws_row;
        int col = size.ws_col;

        blankspace = (col - 28 ) >> 1;
        newline = (row - 10) >> 1;
    }
    
    for(int i = 0; i < newline; ++i)
    {
        cout << endl;
    }

    cout << setw(blankspace) << ' '
        << "         welcome to" << endl
        << setw(blankspace) << ""
        << "use the simple file transfer" << endl
        << setw(blankspace) << ""
        << "   $ have a nice trip $" << endl
        << setw(blankspace) << ""
        << "                    @_@ caft" << endl
        << setw(blankspace) << ""
        << "   email: caft0505@gmail.com" << endl
        << endl;

    cout << setw(blankspace) << ""
        << "press [ENTER] to continue...";

    while('\n' != cin.get())
    {
        continue;
    }

    printf("\033[1;1H\033[2J");
}

int main()
{
    int connfd = my_socket::create_socket_for_client();
    client cli(connfd);

    welcome();

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
