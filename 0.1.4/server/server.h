#ifndef _SERVER_H_
#define _SERVER_H_

#include "../afxstd.h"
#include "../protocol.h"
#include "../ftp/ftp.h"
#include "../guider/guider.h"

class server: public ftp
{
public:
    server(int fd, const char * root_path):
                            m_current_state(EN_RUN),
                                m_guider(root_path),
                                            ftp(fd)
    { ; }

    ~server() {}

private:
    int deal_upload();
    int deal_download();

private:
    int _delete(const char * name);

private:
    int  ls();
    void cd();
    void pwd();
    int  rm();
    int  mkdir();

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
