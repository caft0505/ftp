#include "guider.h"

guider::guider(const char * original_path = NULL)
{
    m_current_location = EN_LOCAL;
    m_root_path = original_path;
    m_current_path = m_root_path;
    m_old_path = m_root_path;

    if( -1 == chdir(m_current_path) )
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
    }
}

int guider::pwd()
{
    if( !m_current_path.empty() )
    {
        cerr << "unknow guider" << endl;
        return -1;
    }

    cout << m_current_path << endl;

    return 0;
}

int guider::test_add_path(string new_dir)
{
    int res = 0;
    int pos = -1;
    string remain;

    while( !new_dir.empty()
            && ( (pos = new_dir.find("..")) > -1) )
    {
        // Such as "cd ../"
        // "cd /../"
        if(pos < 2)
        {
            if(m_current_path == m_root_path)
            {
                res = -1;
                break;
            }

            // Such as " cd ../../"
            if(new_dir.length() > 3)
            {
                new_dir = new_dir.substr(3,
                        new_dir.length() - 3);
            }
            // Such as " cd ../"
            else
            {
                new_dir.erase(0, new_dir.length());
            }

            pos = m_current_path.find_last_of("/",
                    m_current_path.length() - 1);

            m_current_path.resize(pos + 1);
        }
        // Such as "cd ./test/../"
        else
        {
            remain.erase(0, remain.length());

            // Such as "cd ./test/../test/"
            if(new_dir.length() > pos + 3)
            {
                remain = new_dir.substring(pos + 3,
                        new_dir.length() - pos - 3 );
            }

            if(pos - 2 > 0)
            {
                pos = new_dir.find_last_of("/", pos - 2);
            }
            // Such as "cd /.."
            else
            {
                pos = 0;
            }

            new_dir.resize(pos + 1);
            new_dir += remain;
        }
    }

    m_current_path += new_dir;

    return res;
}

int guider::cd(const string & new_dir)
{
    int res = 0;

    if( is_at_local() )
    {
        // Remeber the old guider for recover.
        m_old_path = m_current_path;

        if( '/' != *(new_dir.end() - 1) )
        {
            new_dir += "/";
        }

        if( -1 == test_add_path(new_dir) )
        {
            m_current_path = m_old_path;
        }

        cout << m_current_path << endl;

        if( -1 == chdir(m_current_path) )
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

            m_current_path.clear();
            m_current_path = m_old_path;

            res = -1;
        }
    }
    else
    {
        cout << "send guider to remote" << endl;// Send guider to remote.
    }

    return res;
}
