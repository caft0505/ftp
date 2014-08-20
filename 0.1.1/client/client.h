#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../afxstd.h"
#include "../protocol.h"
#include "../ftp/ftp.h"
#include "../command/command.h"
#include "../guider/guider.h"
#include "../tools/ls/ls.h"

#define PATH    "/home/caft/Desktop/ftp/client_path/"
#define LOCAL   "local "
#define REMOTE  "remote "

class client: public ftp
{
public:
    client(int fd, const char *root_path = PATH):
                        m_current_state(EN_RUN),
                        ftp( fd),
                        m_guider(root_path)
    {
        init_command();
    }

    ~client() { _free(); }

private:
    void init_command();
    void _free();

public:
    int is_command_type(unsigned char dst_cmd_type,
                        unsigned char src_cmd_type)
    {
        return (dst_cmd_type == src_cmd_type);
    }

    int is_run()
    {
        return ( EN_RUN == m_current_state );
    }

    int is_over()
    {
        return (EN_OVER == m_current_state);
    }

public:
    void clear()
    {
        printf("\033[1;1H\033[2J");
    }

    int cd();

    int pwd();

    void cdlocal()
    {
        m_guider.cdlocal();
    }

    void cdremote()
    {
        m_guider.cdremote();
    }

    int ls();

    void over()
    {
        cout << "Bye-Bye..." << endl;
        m_current_state = EN_OVER;
    }

public:
    void get_cmd();
    int  dispatch();

private:
    // Is 1, the client is running.
    // Is 0, the client is over.
    // 1 is default state.
    enum
    {
        EN_OVER = 0,
        EN_RUN  = 1
    } m_current_state;

    deque<char *> m_cmd;

    command m_command_checker;

    guider m_guider;

};

#endif
