#include "socket.h"

int my_socket::get_IP()
{
    return inet_addr("127.0.0.1");
}

int my_socket::get_port()
{
    return htons(6500);
}

int my_socket::create_socket_for_server()
{
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = get_port();
    saddr.sin_addr.s_addr = get_IP();

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd > 0);

    int res = 0;

    res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
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
