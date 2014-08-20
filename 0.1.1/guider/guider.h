#ifndef _GUIDER_H_
#define _GUIDER_H_

#include "../afxstd.h"

#define BUFFSIZE    1024

class guider
{
public:
    guider(const char * original_path = NULL);

    ~guider() {}

private:
    int is_out_of_root_path();

public:
    const char * get_title_info();

public:
    int pwd();
    int cd(const char * new_dir);

public:
    void cdlocal()
    {
        m_current_location = EN_LOCAL;
    }

    void cdremote()
    {
        m_current_location = EN_REMOTE;
    }

public:
    int is_at_local()
    {
        return (EN_LOCAL == m_current_location);
    }

    int is_at_remote()
    {
        return (EN_REMOTE == m_current_location);
    }

private:
    enum
    {
        EN_LOCAL = 0,
        EN_REMOTE = 1
    } m_current_location;

    string m_current_path;
    string m_old_path;

    string m_root_path;
};

#endif
