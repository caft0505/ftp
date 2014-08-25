#include "guider.h"

guider::guider(const char * original_path)
{
    m_current_location = EN_LOCAL;
    m_root_path = original_path;
    m_current_path = m_root_path;
    m_old_path = m_root_path;

    if( -1 == chdir(m_current_path.c_str()) )
    {
        if( (ENOTDIR == errno)
                || (ENOENT == errno) )
        {
            cerr << "' " << m_current_path << " '"
                << "is not exist or not a directory"
                << endl;
        }
        else
        {
            cerr << "error chdir in "
                << __FILE__ << " : "
                << __LINE__ << endl;
        }
    }
}

char * guider::pwd(char * path, int len)
{
    if( is_at_remote() )
    {
        return NULL;
    }

    if(NULL == path || len <= 0)
    {
        return NULL;
    }

    if( m_current_path.empty() )
    {
        cerr << "unknow path" << endl;
        return NULL;
    }

    int pos = m_current_path.size() - m_root_path.size();

    string tmp = "~/";

    if(pos > 0)
    {
        tmp =  "~/" +  m_current_path.substr(
                        m_current_path.size() - pos, pos);
    }

    bzero(path, len);
    tmp.copy(path, (tmp.size() > len ? len : tmp.size()), 0);

    return path;
}

int guider::is_out_of_root_path()
{
    char buff[BUFFSIZE] = {0};

    if( (NULL == getcwd(buff, BUFFSIZE)) )
    {
        cerr << "error getcwd in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        return -1;
    }

    m_current_path.erase( m_current_path.begin(),
            m_current_path.end() );

    m_current_path = buff;
    m_current_path += "/";

    return (-1 == (int)m_current_path.find(m_root_path) );
}

int guider::cd(const char * new_dir)
{
    if( is_at_remote() )
    {
        return -1;
    }

    if( (NULL == new_dir)
        || (0 == strcmp(new_dir, "~" )) )
    {
        new_dir = m_root_path.c_str();
    }

    // Remeber the old guider for recover.
    m_old_path = m_current_path;

    if( -1 == chdir(new_dir) )
    {
        if( (ENOTDIR == errno)
                || (ENOENT == errno) )
        {
            cerr << "' " << new_dir << " '"
                << "is not exist or not a directory"
                << endl;
        }
        else
        {
            cerr << "error chdir in "
                << __FILE__ << " : "
                << __LINE__ << endl;
        }

        m_current_path.erase(0, m_current_path.length() );
        m_current_path = m_old_path;

        return -1;
    }

    if( is_out_of_root_path() )
    {
        m_current_path.erase(0, m_current_path.length() );
        m_current_path = m_root_path;

        if( -1 == chdir(m_current_path.c_str()) )
        {
            if( (ENOTDIR == errno)
                    || (ENOENT == errno) )
            {
                cerr << "' " << m_current_path << " '"
                    << "is not exist or not a directory"
                    << endl;
            }
            else
            {
                cerr << "error chdir in "
                    << __FILE__ << " : "
                    << __LINE__ << endl;
            }

            return -1;
        }
    }

    return 0;
}
