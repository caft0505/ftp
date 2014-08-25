#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../afxstd.h"
#include "../protocol.h"
#include "../ftp/ftp.h"
#include "../command/command.h"
#include "../guider/guider.h"
#include "../tools/ls/ls.h"

#define LOCAL   "local "
#define REMOTE  "remote "
#define YES     "yes"
#define NO      "no"

class client: public ftp
{
public:
    client(int fd, const char *root_path):
                        m_current_state(EN_RUN),
                            m_guider(root_path),
                                        ftp( fd)
    {
        init_command();
    }

    ~client() { _free(); }

private:
    void init_command();
    void _free();
    int _delete(const char * name);

public:
    static int is_command_type(unsigned char dst_cmd_type,
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

private:
    int file_exist_in_remote(const char * file_name);

    static int is_choice(const char * std, const char * choice);

    static int is_yes(const char * choice)
    {
        return is_choice(YES, choice);
    }

    static int is_no(const char * choice)
    {
        return is_choice(NO, choice);
    }

public:
    int upload();
    int download();

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

    int rm();

    int mkdir();

    void over();

public:
    char * get_title_info(char * info, int len);

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
