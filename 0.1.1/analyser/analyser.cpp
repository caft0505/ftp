#include "analyser.h"

void analyser::load_data(const char * pbuff, int len)
{
    if ( (NULL != pbuff) || (len <= 0) )
    {
        cerr << "error arguments in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        return ;
    }

    m_pack.clear();

    m_pack.insert( m_pack.begin(), pbuff,
                            pbuff + len );
}

void analyser::load_data(const deque<char> & buff)
{
    m_pack.clear();

    m_pack.insert( m_pack.begin(), buff.begin(),
                                    buff.end() );
}

int analyser::checkout()
{
    if(m_pack.empty())
    {
        cerr << "error checkout in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    int checkcode = 0;
    
    for(int i = 0; i < m_pack.size() - 1; ++i)
    {
        checkcode ^= m_pack[i];
    }

    return (checkcode == *( m_pack.end() ) );
}

char analyser::check()
{
    if(m_pack.empty())
    {
        cerr << "error check in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    int checkcode = 0;

    deque<char>::iterator it_p = m_pack.begin();

    for( ; it_p != m_pack.end(); ++it_p)
    {
        checkcode ^= *it_p;
    }

    return checkcode;
}

char analyser::get_pack_type()
{
    if(m_pack.empty())
    {
        cerr << "error reference a empty value in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    return m_pack[0];
}

u_short analyser::get_pack_size()
{
    if(m_pack.empty())
    {
        cerr << "error reference a empty value in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    char buff[sizeof(short)] = {0};

    for(int i = 0; i < sizeof(short); ++i)
    {
        buff[i] = m_pack[1 + i];
    }

    return (parse_int(buff, sizeof(short)));
}

u_short analyser::get_fname_len()
{
    if(m_pack.empty())
    {
        cerr << "error reference a empty value in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    char buff[sizeof(short)] = {0};

    for(int i = 0; i < sizeof(short); ++i)
    {
        buff[i] = m_pack[5 + i];
    }

    return (parse_short(buff, sizeof(short)));
}

int analyser::get_file_name(char * file_name, int len) 
{
    if(m_pack.empty())
    {
        cerr << "error reference a empty value in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    if( (NULL == file_name) || (len <= 0) )
    {
        cerr << "error arguments in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        return -1;
    }

    int file_name_len = get_fname_len();
    if(file_name_len > len)
    {
        cerr << "error file_name's len is too short"
            << endl;
        return -1;
    }

    bzero(file_name, len);

    for(int i = 0; i < file_name_len; ++i)
    {
        file_name[i] = m_pack[7 + i];
    }

    return 0;
}

u_int analyser::get_file_size()
{
    if(m_pack.empty())
    {
        cerr << "error reference a empty value in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    char buff[sizeof(int)] = {0};
    int start_index = 1 + 4 + 2 + get_fname_len();

    for(int i = 0; i < sizeof(int); ++i)
    {
        buff[i] = m_pack[start_index + i];
    }

    return (parse_int(buff, sizeof(int)));
}

int analyser::get_file_mode()
{
    if(m_pack.empty())
    {
        cerr << "error reference a empty value in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        exit(1);
    }

    char buff[sizeof(int)] = {0};
    int start_index = 1 + 4 + 2 +
                    get_fname_len() + 4;

    for(int i = 0; i < sizeof(int); ++i)
    {
        buff[i] = m_pack[start_index + i];
    }

    return (parse_int(buff, sizeof(int)));
}
