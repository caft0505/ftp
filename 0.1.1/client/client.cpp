#include "client.h"

void client::init_command()
{
    m_command_checker.register_command("upload", CMD_UPLD);
    m_command_checker.register_command("download", CMD_DWLD);

    m_command_checker.register_command("clear", CMD_CLEAR);
    m_command_checker.register_command("ls", CMD_LS);
    m_command_checker.register_command("ll", CMD_LS);
    m_command_checker.register_command("rm", CMD_RM);
    m_command_checker.register_command("cd", CMD_CD);
    m_command_checker.register_command("cdlocal", CMD_CDLOCAL);
    m_command_checker.register_command("cdremote", CMD_CDREMOTE);
    m_command_checker.register_command("pwd", CMD_PWD);
    m_command_checker.register_command("exit", CMD_EXIT);
    
}

void client::_free()
{
    for(int i = 0; i < m_cmd.size(); ++i)
    {
        free(m_cmd[i]);
    }
}

int client::cd()
{   
    const char * path = ( (m_cmd.size() > 1) ?
                                m_cmd[1] : "~");

    int res = 0;

    if( m_guider.is_at_local() )
    {
        res = m_guider.cd( path );
    }
    else
    {
        ;
    }

    return res;
}

int client::pwd()
{
    int res = 0;

    if( m_guider.is_at_local() )
    {
        res = m_guider.pwd();
    }
    else
    {
        ;
    }

    return res;
}

int client::ls()
{
    int res = 0;

    const char *tmp[BUFFSIZE] = {0};

    if( m_guider.is_at_local() )
    {
        for(int i = 0; i < m_cmd.size() && i < 10; ++i)
        {
            tmp[i] = m_cmd[i];
        }

        ::ls(tmp);
    }
    else
    {
        ;
    }

    return res;
}

void client::get_cmd()
{
    char cmd[BUFFSIZE] = {0};

    do
    {
        cout << "[ftp<*~ caft ~*>@"
            << (m_guider.is_at_local() ? LOCAL : REMOTE)
            << m_guider.get_title_info()
            << "] >> ";
        cout.flush();

        cin.getline(cmd, BUFFSIZE);

    }while(cmd[0] == '\0');

    // Free the malloc memory.
    _free();
    // Clear the data in deque.
    m_cmd.clear();

    char * p = strtok(cmd, " ");
    char *q = NULL;
    while( NULL != p )
    {
        if( NULL == (q = strdup(p)) )
        {
            cerr << "error strdup in "
                << __FILE__ << " : "
                << __LINE__ << endl;
            exit(1);
        }

        m_cmd.push_back(q);

        p = strtok(NULL, " ");
    }
}

int client::dispatch()
{
    char cmd_type =
        m_command_checker.get_command_type(m_cmd[0]);

    if( is_command_type(CMD_ERR, cmd_type) )
    {
        return -1;
    }

    int res = 0;

    switch(cmd_type)
    {
        case CMD_CLEAR:
            clear();
            break;

        case CMD_LS:
            res = ls();
            break;

        case CMD_CD:
            cd();
            break;

        case CMD_CDLOCAL:
            cdlocal();
            break;

        case CMD_CDREMOTE:
            cdremote();
            break;

        case CMD_RM:
            break;

        case CMD_PWD:
            pwd();
            break;

        case CMD_EXIT:
            over();
            break;

        case CMD_UPLD:
            break;

        case CMD_DWLD:
            break;

        default:
            break;
    }


    return res;
}
