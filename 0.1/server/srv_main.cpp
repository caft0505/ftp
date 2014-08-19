#include "../processpool/processpool.h"
#include "../socket/socket.h"
#include "server.h"

struct arg_info
{
    int id;
    processpool * p_pro_pool;

}ar_in[DFLNUM];

/*
void global_process_fun(void * arg)
{
    if(NULL == arg)
    {
        cerr << "arg is null" << endl;
        return ;
    }

    server srv( *(int *)arg );
}
*/

void global_process_fun(void * arg)
{
    int connfd = *(int *)arg;

    while( 1 )
    {
        char buff[BUFSIZ] = {0};
        int res = 0;

        res = recv(connfd, buff, BUFSIZ, 0);
        if(0 == res)
        {
            break;
        }
        else if(res < 0)
        {
            cerr << "recv error" << endl;
            break;
        }

        if(0 == strcmp(buff, "exit"))
        {
            break;
        }
        
        cout << buff << endl;

        res = send(connfd, "OK", 2, 0);
        if(res < 0)
        {
            cout << "send error" << endl;
            break;
        }
    }
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

    int sockfd = my_socket::create_socket_for_server();

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
