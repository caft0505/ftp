#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "../afxstd.h"

const int SERVER = 1;
const int CLIENT = 0;

class my_socket
{
public:
    my_socket(int type = SERVER): m_type(type) {}

private:
    static int get_IP();
    static int get_port();

public:
    static int create_socket_for_server();
    static int create_socket_for_client();

private:
    int m_type;
};

#endif
