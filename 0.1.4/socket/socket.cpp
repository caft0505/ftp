#include "socket.h"

char my_socket::m_IP[BUFSIZ] = {0};
char my_socket::m_port[BUFSIZ] = {0};

int my_socket::get_socket_addr()
{
    static int flg = 0;

    if(1 == flg)
    {
        return 0;
    }

    flg = 1;

    bzero(my_socket::m_IP, BUFSIZ);
    bzero(my_socket::m_port, BUFSIZ);

    char buff[BUFSIZ] = {0};
    char *p = NULL;

    FILE * cfp = fopen("auto.conf", "r");
    if(NULL != cfp)
    {
        while( NULL != fgets(buff, BUFSIZ, cfp) )
        {
            // Ignore the new_line at tail.
            buff[strlen(buff) - 1] = '\0';

            if( NULL != (p = strchr(buff, ':') ) )
            {
                if(0 == strncmp(buff, "IP", p - buff))
                {
                    if(strlen(p + 1) > 0)
                    {
                        strcpy(my_socket::m_IP, p + 1);
                    }
                }
                else if( 0 == strncmp(buff, "PORT", p - buff) )
                {
                    if(strlen(p + 1) > 0)
                    {
                        strcpy(my_socket::m_port, p + 1);
                    }
                }
            }

            bzero(buff, BUFSIZ);
        }

        fclose(cfp);

        return 0;
    }
    else
    {
        perror("error fopen");
        return -1;
    }
}


const char * my_socket::get_IP_from_host()
{
    int s;
    struct ifconf conf;
    struct ifreq *ifr;
    char buff[BUFSIZ];
    int num;
    int i = 0;

    s = socket(PF_INET, SOCK_DGRAM, 0);

    conf.ifc_len = BUFSIZ;
    conf.ifc_buf = buff;

    ioctl(s, SIOCGIFCONF, &conf);

    num = conf.ifc_len / sizeof(struct ifreq);
    ifr = conf.ifc_req;

    struct sockaddr_in * sin = NULL;

    for(i=0; i < num; ++i)
    {
        sin = (struct sockaddr_in *)(&ifr->ifr_addr);

        ioctl(s, SIOCGIFFLAGS, ifr);

        if( ((ifr->ifr_flags & IFF_LOOPBACK) == 0)
                        && (ifr->ifr_flags & IFF_UP) )
        {
            //printf("%s (%s)\n",
            //        ifr->ifr_name,
            //        inet_ntoa(sin->sin_addr));
            
            return inet_ntoa(sin->sin_addr);
        }

        ifr++;
    }

    return STD_IP_STR;
}

const char * my_socket::get_IP_from_conf()
{
    if(-1 == get_socket_addr())
    {
        return NULL;
    }

    return ( strlen(my_socket::m_IP) > 0 ?
                    my_socket::m_IP : NULL );
}

const char * my_socket::get_port_from_conf()
{
    if(-1 == get_socket_addr())
    {
        return NULL;
    }

    return (strlen(my_socket::m_port) > 0 ?
                    my_socket::m_port: NULL);
}

int my_socket::get_port()
{
    const char *p = get_port_from_conf();
    if(NULL != p)
    {
        return htons( atoi(p) );
    }

    return htons(STD_PORT);
}

int my_socket::get_IP()
{
    const char *p = get_IP_from_conf();

    if(NULL != p)
    {
        return inet_addr(p);
    }

    return inet_addr( get_IP_from_host() );
}

int my_socket::create_socket_for_server()
{
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = get_port();
    saddr.sin_addr.s_addr = get_IP();

    cout << "IP: " << inet_ntoa(saddr.sin_addr) << endl
        << "port: " << ntohs(saddr.sin_port)
        << endl << endl;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd > 0);

    int res = 0;

    res = bind(sockfd, (struct sockaddr *)&saddr,
                                    sizeof(saddr));
    assert(-1 != res);

    res = listen(sockfd, 5);
    assert(-1 != res);

    return sockfd;
}

int my_socket::create_socket_for_client()
{
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = get_port();
    saddr.sin_addr.s_addr = get_IP();

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd > 0);

    int res = 0;

    res = connect(sockfd, (struct sockaddr *)&saddr,
            sizeof(saddr));
    assert(-1 != res);

    return sockfd;
}
