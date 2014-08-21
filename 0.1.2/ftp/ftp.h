#ifndef _FTP_H_
#define _FTP_H_

#include "../afxstd.h"
#include "../protocol.h"
#include "../analyser/analyser.h"

#define BUFFSIZE 1024

class ftp
{
public:
    ftp() {}

    ftp(int fd): m_fd(fd) {}

    ~ftp() {}

private:
    typedef struct
    {
        char    file_name[BUFFSIZE];
        off_t   file_size;
        mode_t  file_mode;

    }file_info;

    int recv_file_info(file_info &f_info);
    
    int send_file_info(const file_info &f_info);

public:
    int recv_data();
    int send_data();

    int recv_file();
    int send_file(char * file_name);

public:
    int get_buff(char * buff, int blen);
    int set_buff(const char * buff, int blen);
    int add_buff(const char * buff, int blen);

public:
    int get_fd() { return m_fd; }
    void set_fd(int fd) { m_fd = fd; }

public:
    // Reply a package.
    int reply(char pack_type);

    // If the acceptable pack is PT_OK it returns 1,
    // else return 0
    int acceptable();

    int is_pack_type(unsigned char dst_pack_type, 
                        unsigned char src_pack_type)
    {
        return (dst_pack_type == src_pack_type);
    }

protected:
    int         m_fd;
    deque<char> m_buff;
    analyser    m_analyser;

};

#endif
