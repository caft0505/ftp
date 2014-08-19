#ifndef _SERVER_H_
#define _SERVER_H_

#include "../afxstd.h"
#include "../protocol.h"
#include "../ftp/ftp.h"
#include "../command/command.h"

#define PATH    "/home/caft/Desktop/ftp/server_file/"

class server: public ftp
{
public:
    server(int fd, const char * root_work_path = PATH):
                                ftp(fd, root_work_path)
    {
        bzero(m_root_work_path, BUFFSIZE);
        strncpy(m_root_work_path, root_work_path, BUFFSIZE);
    }

    ~server() {}

public:
    void deal_cmd();
    int  dispatch();

private:
    deque<char *> m_cmd;

    command m_command_checker;

    char m_root_work_path[BUFFSIZE];

};

#endif
