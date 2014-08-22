#ifndef _SERVER_H_
#define _SERVER_H_

#include "../afxstd.h"
#include "../protocol.h"
#include "../ftp/ftp.h"
#include "../guider/guider.h"

#define PATH    "/home/caft/Desktop/ftp/server_path/"

class server: public ftp
{
public:
    server(int fd, const char * root_path = PATH):
                            m_current_state(EN_RUN),
                                m_guider(root_path),
                                            ftp(fd)
    { ; }

    ~server() {}

private:
    int deal_upload();
    int deal_download();

private:
    int  ls();
    void cd();
    void pwd();
    //int rm();

private:
    void over()
    {
        m_current_state = EN_OVER;
    }

public:
    int is_run()
    {
        return (EN_RUN == m_current_state);
    }

public:
    int deal_cmd();
    int dispatch();

private:
    enum
    {
        EN_OVER = 0,
        EN_RUN  = 1
    }m_current_state;

    guider m_guider;

};

#endif
