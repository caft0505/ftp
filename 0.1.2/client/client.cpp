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
        m_buff.clear();

        // Add the path into m_buff.
        set_buff(path, strlen(path));

        // Add the pack type.
        m_buff.push_front(PT_CD);

        // Add the checkcode.
        m_analyser.load_data(m_buff);
        m_buff.push_back(m_analyser.check() );

        if(-1 == send_data() )
        {
            ;// Send pack error.
        }

        if( !acceptable() )
        {
            cout << "' " << path << " '"
                << "is not exist or not a directory"
                << endl;
        }
    }

    return res;
}

int client::pwd()
{
    int res = 0;
    char path[BUFFSIZE] = {0};

    if( m_guider.is_at_local() )
    {
        cout << (NULL != m_guider.pwd(path, BUFFSIZE) ?
                                            path : " ")
            << endl;
    }
    else
    {
        reply(PT_PWD);
        if( !acceptable() )
        {
            ;// Send pack error.
        }

        if( -1 == recv_data() )
        {
            ;// Error
        }

        // Delete the pack type.
        m_buff.pop_front();

        // Delete the data size.
        m_buff.pop_front();
        m_buff.pop_front();

        get_buff(path, BUFFSIZE);

        cout << path << endl;
        
    }

    return res;
}

int client::ls()
{
    const char *tmp[BUFFSIZE] = {0};

    if( m_guider.is_at_local() )
    {
        for(int i = 0; i < m_cmd.size() && i < 10; ++i)
        {
            tmp[i] = m_cmd[i];
        }

        // Call the global function ls to show the
        // information of the specified path in local.
        if(-1 == ::ls(tmp) )
        {
            return -1;
        }
    }
    else
    {
        m_buff.clear();

        // Add the pack type.
        m_buff.push_front(PT_LS);

        // If it has a parameter.
        if(m_cmd.size() > 1)
        {
            // Add the dir-name
            m_buff.insert(m_buff.end(), m_cmd[1],
                    m_cmd[1] + strlen(m_cmd[1]));
        }
        else
        {
            // Show current directory.
            m_buff.push_back('.');
        }

        // Init the analyser.
        m_analyser.load_data(m_buff);
        // Add the checkcode into the tail of buff.
        m_buff.push_back(m_analyser.check());

        if( -1 == send_data() )
        {
            return -1;
        }

        if(-1 == recv_data() )
        {
            return -1;
        }

        // Delete the pack type.
        m_buff.pop_front();
        // Delete the data len.
        m_buff.pop_front();
        m_buff.pop_front();
        // Replace checkcode with '\0' to print.
        m_buff[m_buff.size() - 1] = '\0';

        // Print the data.
        char buff[BUFFSIZE] = {0};
        get_buff(buff, BUFFSIZE);

        cout << buff << endl;

    }

    return 0;
}

char * client::get_title_info(char * info, int len)
{
    if(NULL == info || len <= 0)
    {
        cerr << "error arguments in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        return NULL;
    }

    char path[BUFFSIZE] = {0};

    if(m_guider.is_at_local() )
    {
        if(NULL == m_guider.pwd(path, BUFFSIZE))
        {
            return NULL;
        }
    }
    else
    {
        reply(PT_PWD);
        if( !acceptable() )
        {
            ;// Send pack error.
        }

        if( -1 == recv_data() )
        {
            ;// Error
        }

        // Delete the pack type.
        m_buff.pop_front();

        // Delete the data size.
        m_buff.pop_front();
        m_buff.pop_front();

        get_buff(path, BUFFSIZE);
    }

    bzero(info, len);

    path[strlen(path) - 1] = '\0';
    char *pstr = strrchr(path, '/');
    if(NULL != pstr)
    {
        strncpy(info, pstr + 1, len);
    }
    else
    {
        strcpy(info, "~");
    }

    return info;
}

void client::get_cmd()
{
    char cmd[BUFFSIZE] = {0};
    char info[BUFFSIZE] = {0};

    do
    {
        cout << "[ftp<*~ caft ~*>@"
            << (m_guider.is_at_local() ? LOCAL : REMOTE)
            << (NULL != get_title_info(info, BUFFSIZE) ?
                                                info : "" )
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

void client::over()
{
    cout << "Bye-Bye..." << endl;
    m_current_state = EN_OVER;

    reply(PT_EXIT);
}
