#ifndef _GUIDER_H_
#define _GUIDER_H_

class guider
{
public:
    guider(const char * original_path = NULL);

    ~guider() {}

private:
    int test_add_path(string new_dir);

public:
    int pwd();
    int cd(const string & new_dir);

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
