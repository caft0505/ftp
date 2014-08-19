#include "processpool.h"

static const char BUFFSIZE = 32;
void process::send_fd(int sockfd, int sendfd)
{
    struct iovec iov[1];
    struct msghdr msg;
    char buff[BUFFSIZE] = {0};
     
    iov[0].iov_base = buff;
    iov[0].iov_len = 1;
   
    msg.msg_name  = NULL;
    msg.msg_namelen = 0;

    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    struct  cmsghdr cm;

    cm.cmsg_len = CMSG_LEN(sizeof(int));
    cm.cmsg_level = SOL_SOCKET;
    cm.cmsg_type = SCM_RIGHTS;
    
    *(int *)CMSG_DATA(&cm) = sendfd;
     
    msg.msg_control = &cm;
    msg.msg_controllen = CMSG_LEN(sizeof(int));
  
    assert(sendmsg(sockfd,&msg,0) != -1);
}

int process::recv_fd(int sockfd)
{
   struct iovec iov[1];
   struct msghdr msg;
   char buff[BUFFSIZE];
   
   iov[0].iov_base = buff;
   iov[0].iov_len = 1;
 
   msg.msg_name = NULL;
   msg.msg_namelen = 0;

   msg.msg_iov = iov;
   msg.msg_iovlen = 1;

   struct cmsghdr cm;
   msg.msg_control = &cm;
   msg.msg_controllen = CMSG_LEN(sizeof(int));
 
   assert( recvmsg(sockfd,&msg,0) != -1);

   return (*(int*)CMSG_DATA(&cm));
}

void process::run()
{
    while( 1 )
    {
        int task_fd;
        task_fd = recv_fd(m_fd);

        m_pfun(&task_fd);

        close(task_fd);

        if(4 != send(m_fd, "OVER", 4, 0))
        {
            ;// Error handling.
        }
    }
}

processpool::processpool(int num): m_pprocess(NULL),
                                   m_process_num(num)
{
    m_pprocess = new process[m_process_num];
    if(NULL == m_pprocess)
    {
        cerr << "bad alloc" << endl;
        exit(1);
    }

    m_wait_list.clear();
}

void processpool::set_hook(process_pfun new_pfun,
                                            int id)
{
    if(id < 0 || id >= m_process_num)
    {
        cerr << "out of range" << endl;
        return ;
    }

    m_pprocess[id].set_hook(new_pfun);
}

void processpool::set_hook(process_pfun new_pfun)
{
    for(int i = 0; i < m_process_num; ++i)
    {
        set_hook(new_pfun, i);
    }
}

int processpool::create_processpool()
{
    int fd[2];
    int i = 0;

    for( ; i < m_process_num; ++i)
    {
        assert( -1 != socketpair(AF_UNIX,
                        SOCK_STREAM, 0, fd) );
        
        pid_t pid = fork();
        assert(-1 != pid);

        if(0 == pid)
        {
            close(fd[0]);
            
            m_pprocess[i].set_fd(fd[1]);
            m_pprocess[i].run();

            exit(0);
        }

        close(fd[1]);
        m_pprocess[i].set_id(i);
        m_pprocess[i].set_fd(fd[0]);
        m_pprocess[i].set_free();
    }

    show_process_stat();

    return 0;
}

int processpool::get_a_free_process()
{
    int i = 0;

    for( ; (i < m_process_num)
        && (m_pprocess[i].is_busy()); ++i)
    {
        ;
    }

    return ( (i < m_process_num) ? i : -1 );
}

int processpool::deal(void * arg)
{
    int id_free = get_a_free_process();

    if(NULL != arg)
    {
        if(-1 != id_free)
        {
            cout << "child process(ID): " << id_free
                << " >> accept the task" << endl;
                
            m_pprocess[id_free].set_busy();
            m_pprocess[id_free].wakeup(arg);
        }
        else
        {
            m_wait_list.push_back(*(int *)arg);
        }
    }
    else if( (!m_wait_list.empty())
                    && (-1 != id_free) )
    {
        int waited_fd = m_wait_list.front();
        m_wait_list.pop_front();

        cout << "child process(ID): " << id_free
            << " >> accept the task" << endl;
                
        m_pprocess[id_free].set_busy();
        m_pprocess[id_free].wakeup(&waited_fd);
    }

    show_process_stat();

    return 0;
}

void processpool::show_process_stat()
{
    cout << endl
        << "free process(ID):" << endl;

    int i = 0;
    for(i = 0; i < m_process_num; ++i)
    {
        if(m_pprocess[i].is_free())
        {
            cout << left << setw(5) << i;
        }
    }

    cout << endl
        << "num of waiting task: "
        << m_wait_list.size()
        << endl << endl;
}
