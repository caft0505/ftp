#ifndef _PROCESSPOOL_H_
#define _PROCESSPOOL_H_

#include "../afxstd.h"

const int DFLNUM = 3;
const int BUSY = 1;
const int FREE = 0;

typedef void (*process_pfun)(void * arg);

class process
{
public:
    process():m_id(-1), m_fd(-1),
            m_stat(FREE), m_pfun(NULL) {}

    ~process() {}

private:
    static void send_fd(int sockfd, int sendfd);
    static int  recv_fd(int sockfd);

public:
    void set_fd(int fd)
    {
        m_fd = fd;
    }

    int get_fd()
    {
        return m_fd;
    }

    void set_id(int id)
    {
        m_id = id;
    }

    int get_id()
    {
        return m_id;
    }

    void set_busy()
    {
        m_stat = BUSY;
    }

    void set_free()
    {
        m_stat = FREE;
    }

    int get_stat()
    {
        return m_stat;
    }

    int is_free()
    {
        return ( FREE == get_stat() );
    }

    int is_busy()
    {
        return ( !is_free() );
    }

    void set_hook(process_pfun new_pfun)
    {
        m_pfun = new_pfun;
    }

    void wakeup(void * arg)
    {
        send_fd(m_fd, *(int *)arg );
    }

    void run();

private:
    int             m_id;

    // Use for both child process and
    // parent process to communicate
    // with each other.
    int             m_fd;

    int             m_stat;
    process_pfun    m_pfun;

};

class processpool
{
public:
    processpool(int num = DFLNUM);

    ~processpool()
    {
        delete []m_pprocess;
    }

public:
    // This two function are used to hook the task.
    // They must be called before calling create_processpool.
    void set_hook(process_pfun new_pfun, int id);
    void set_hook(process_pfun new_pfun);

    // Create the processpool with num of m_process_num.
    // Must call set_hook function before calling this
    // function.
    int create_processpool();

    // Deal a new task.
    int deal(void * arg = NULL);

    // Get a free process.
    // Return a free index from processpool.
    int get_a_free_process();

    process & operator[](int id)
    {
        if(id < 0 || id > m_process_num)
        {
            cerr << "out of range" << endl;
            exit(1);
        }

        return m_pprocess[id];
    }

    void show_process_stat();

private:
    process *   m_pprocess;// Child process information.
    int         m_process_num;// Child process num.
    deque<int>  m_wait_list;// Waited task.

};

#endif
