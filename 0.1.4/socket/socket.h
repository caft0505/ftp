#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "../afxstd.h"

#define STD_IP_STR  ("127.0.0.1")
#define STD_PORT    6500

class my_socket
{
public:
    my_socket() { }

private:
    static int    get_socket_addr();

    static const char * get_IP_from_host();

    static const char * get_IP_from_conf();

    static const char * get_port_from_conf();

public:
    static int get_IP();
    static int get_port();

public:
    static int create_socket_for_server();
    static int create_socket_for_client();

public:
    static char m_IP[BUFSIZ];
    static char m_port[BUFSIZ];

};

#endif
