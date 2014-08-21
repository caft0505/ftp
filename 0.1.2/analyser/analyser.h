#ifndef _ANALYSER_H_
#define _ANALYSER_H_

#include "../afxstd.h"
#include "../parse/parse.h"

class analyser: public parse
{
public:
    analyser() {}

    ~analyser() {}

public:
    void    load_data(const char * pbuff, int len);
    void    load_data(const deque<char>  & buff);
    int     checkout();
    char    check();

    void    clear()
    {
        m_pack.clear();
    }

public:
    char    get_pack_type();

    u_short get_pack_size();

    u_short get_fname_len();
    int     get_file_name(char * file_name, int len);

    u_int   get_file_size();
    int     get_file_mode();

private:
    deque<char> m_pack;

};

#endif
