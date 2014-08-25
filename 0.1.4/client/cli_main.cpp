#include "client.h"
#include "../socket/socket.h"

#define DFLPATH "/tmp/my_ftp/client_path/"

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

const char * get_work_path(char * work_path, int len)
{
    FILE * fp = fopen("auto.conf", "r");
    if(NULL != fp)
    {
        bzero(work_path, len);
        char buff[BUFFSIZE] = {0};
        char *p = NULL;

        while(NULL != fgets(buff, BUFFSIZE, fp))
        {
            buff[strlen(buff) - 1] = '\0';

            if(NULL != (p = strchr(buff, ':')) )
            {
                if(0 == strncmp(buff, "PATH", p - buff))
                {
                    if(strlen(p + 1) > 0)
                    {
                        strcpy(work_path, p + 1);

                        return work_path;
                    }
                }
            }
        }
        
        fclose(fp);
    }
    else
    {
        perror("error fopen");
    }

    return NULL;
}

int main()
{
    int connfd =
        my_socket::create_socket_for_client();

    char work_path[BUFFSIZE] = {0};
    if(NULL == get_work_path(work_path, BUFFSIZE))
    {
        strcpy(work_path, DFLPATH);
    }
    
    client cli(connfd, work_path);

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
