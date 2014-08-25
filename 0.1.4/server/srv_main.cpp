#include "../processpool/processpool.h"
#include "../socket/socket.h"
#include "server.h"

#define DFLPATH "/tmp/my_ftp/server_path/"

struct arg_info
{
    int id;
    processpool * p_pro_pool;

}ar_in[DFLNUM];

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

void global_process_fun(void * arg)
{
    if(NULL == arg)
    {
        cerr << "arg is null" << endl;
        return ;
    }

    char work_path[BUFFSIZE] = {0};
    if(NULL == get_work_path(work_path, BUFFSIZE))
    {
        strcpy(work_path, DFLPATH);
    }

    server srv( *(int *)arg, work_path );

    do
    {
        if(-1 == srv.deal_cmd() )
        {
            break;
        }

        if( -1 == srv.dispatch() )
        {
            ; // Some errors occur.
        }

    }while( srv.is_run() );
}

void accept_cb(int fd, short ev, void * arg)
{
    int cfd = accept(fd, NULL, NULL);
    if(cfd < 0)
    {
        cerr << "accept error" << endl;
        return ;
    }

    processpool * p_pro_pool = (processpool *)arg;
    p_pro_pool->deal(&cfd);
}

void pipe_cb(int fd, short ev, void * arg)
{
    arg_info *ar_in = (arg_info *)arg;

    cout << endl
        << "child process(ID): " << ar_in->id
        << " >> sends a message: " << endl;

    char buff[BUFSIZ] = {0};
    read(fd, buff, BUFSIZ);
    cout << buff << endl;

    (*(ar_in->p_pro_pool))[ar_in->id].set_free();
    ar_in->p_pro_pool->deal();
}

int main()
{
    processpool p_pool(DFLNUM);
    p_pool.set_hook(global_process_fun);
    p_pool.create_processpool();

    int sockfd =
        my_socket::create_socket_for_server();

    struct event_base * base = event_base_new();
    assert(NULL != base);

    struct event * sock_ev = event_new(base, sockfd,
                                EV_READ | EV_PERSIST,
                                accept_cb, (void *)&p_pool);
    assert(NULL != sock_ev);
    event_add(sock_ev, NULL);

    int i = 0;
    struct event * events[DFLNUM];
    for( ; i < DFLNUM; ++i)
    {
        ar_in[i].id = i;
        ar_in[i].p_pro_pool = &p_pool;

        events[i] = event_new(base, p_pool[i].get_fd(),
                            EV_READ | EV_PERSIST,
                            pipe_cb, (void *)&ar_in[i]);
        assert(NULL != events[i]);

        event_add(events[i], NULL);
    }

    event_base_dispatch(base);

    event_free(sock_ev);
    for(i = 0; i < DFLNUM; ++i)
    {
        event_free(events[i]);
    }
    event_base_free(base);
    cout << "server stop" << endl;

    return 0;
}
