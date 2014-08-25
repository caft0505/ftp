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
    m_command_checker.register_command("mkdir", CMD_MKDIR);

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

    if( m_guider.is_at_local() )
    {
        return m_guider.cd( path );
    }

    m_buff.clear();

    // Add the pack type.
    m_buff.push_front(PT_CD);

    // Add the path into m_buff.
    add_buff(path, strlen(path));

    // Add the checkcode.
    m_analyser.load_data(m_buff);
    m_buff.push_back(m_analyser.check() );

    if(-1 == send_data() )
    {
        return -1;// Send pack error.
    }

    if( acceptable(PT_NOENT) )
    {
        cout << "' " << path << " '"
            << "is not exist or not a directory"
            << endl;
    }

    return 0;
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
        //reply(PT_PWD);
        
        m_buff.clear();
        
        // Add the pack type.
        m_buff.push_front(PT_PWD);

        // Add the checkcode.
        m_buff.push_back(PT_PWD);

        if(-1 == send_data())
        {
            return -1; // Send error.
        }

        // Receive path information.
        if( -1 == recv_data() )
        {
            ;// Error
        }

        if( is_pack_type(PT_DATA,
            m_analyser.get_pack_type()) )
        {
            // Delete the pack type.
            m_buff.pop_front();

            get_buff(path, BUFFSIZE);

            cout << path << endl;
        }
        else
        {
            ; // Uninterested package.
        }

    }

    return res;
}

int client::ls()
{
    const char *tmp[BUFFSIZE] = {0};

    if( m_guider.is_at_local() )
    {
        // For local 'ls', it can show more
        // than one directory.
        //
        for(int i = 0; i < m_cmd.size()
                            && i < 10; ++i)
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
        // For remote 'ls', it can show just
        // only one directory.
        // 
        m_buff.clear();

        // Add the pack type.
        m_buff.push_front(PT_LS);

        // If it has a parameter.
        if(m_cmd.size() > 1)
        {
            // Add the dir-name
            add_buff(m_cmd[1], strlen(m_cmd[1]));
        }
        else
        {
            // Show current directory.
            m_buff.push_back('.');
        }

        // Add the checkcode into the tail of buff.
        m_analyser.load_data(m_buff);
        m_buff.push_back(m_analyser.check());

        if( -1 == send_data() )
        {
            return -1;
        }

        if(-1 == recv_data() )
        {
            return -1;
        }

        if( is_pack_type(PT_DATA,
            m_analyser.get_pack_type()) )
        {
            // Delete the pack type.
            m_buff.pop_front();

            // Print the data.
            char buff[BUFFSIZE] = {0};
            get_buff(buff, BUFFSIZE);

            cout << buff << endl;
        }
        else
        {
            ; // Uninterested package.
        }
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
        m_buff.clear();

        // Add pack type.
        m_buff.push_front(PT_PWD);

        // Add checkcode.
        m_buff.push_back(PT_PWD);

        if(-1 == send_data() )
        {
            return NULL;
        }

        if( -1 == recv_data() )
        {
            return NULL;// Error
        }

        
        if( is_pack_type(PT_DATA,
            m_analyser.get_pack_type()) )
        {
            // Delete the pack type.
            m_buff.pop_front();

            get_buff(path, BUFFSIZE);
        }
        else
        {
            //path[0] = '?'; // Uninterested package.
        }
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

int client::is_choice(const char * std,
        const char * choice)
{
    if(NULL == choice
            || strlen(choice) <= 0
            || strlen(choice) > 3)
    {
        return 0;
    }

    int i = 0;

    for( ; ('\0' != choice[i])
            && (std[i] == tolower(choice[i]))
            ; ++i)
    { ; }

    return ('\0' == choice[i]);
}

int client::upload()
{
    char file_name[BUFFSIZE] = {0};
    if(m_cmd.size() < 2)
    {
        do
        {
            cout << "file name: ";
            cout.flush();

            cin.getline(file_name, BUFFSIZE - 1);

        }while(file_name[0] == '\0');
    }
    else
    {
        strncpy(file_name, m_cmd[1], BUFFSIZE);
    }

    // Check file's existance in local.
    if(-1 == access(file_name, F_OK) )
    {
        cout << "no such file" << endl;
        return -1;
    }

    int res = 0;

    if( -1 == send_file_info(file_name) )
    {
        ;// Send info error.
    }

    char pack_type = acceptable();
    if( is_pack_type(PT_FEXIST, pack_type) )
    {
        while( 1 )
        {
            char choice[BUFFSIZE] = {0};
            cout << "file exist in remote, "
                << "do you want to cover it ? [yes | no]"
                << " >> ";
            cout.flush();

            cin.getline(choice, BUFFSIZE - 1);

            // Default choice is "yes".
            if('\0' == choice[0])
            {
                strcpy(choice, "yes");
            }

            if( is_no(choice) )
            {
                reply(PT_NOCOVER);
                return 0;
            }
            else if( is_yes(choice) )
            {
                reply(PT_COVER);
                break;
            }
            else
            {
                cout << "input error" << endl;
            }
        }
    }

    res = send_file(file_name);

    return res;
}

int client::download()
{
    int res = 0;

    char file_name[BUFFSIZE] = {0};
    if(m_cmd.size() < 2)
    {
        do
        {
            cout << "file name: ";
            cout.flush();

            cin.getline(file_name, BUFFSIZE);

        }while(file_name[0] == '\0');
    }
    else
    {
        strncpy(file_name, m_cmd[1], BUFFSIZE);
    }

    m_buff.clear();

    // Add the pack type.
    m_buff.push_back(PT_DWREQ);

    // Add the filename.
    add_buff(file_name, strlen(file_name));

    // Add checkcode.
    m_analyser.load_data(m_buff);
    m_buff.push_back(m_analyser.check());

    if(-1 == send_data())
    {
        ;// Send error.
    }

    char pack_type = acceptable();
    if( is_pack_type(PT_OK, pack_type) )
    {
        if(-1 == recv_file_info() )
        {
            return -1;// Recv error.
        }

        res = recv_file();
    }
    else if( is_pack_type(PT_FNOEXIST, pack_type) )
    {
        cout << "file not exist" << endl;;
    }

    return res;
}

//Delete dir or others in this 'name'
//
//Input: name
int client::_delete(const char *name)
{
    if(NULL == name)
    {
        return 0;
    }

    struct stat buff = {0};

    assert(NULL != name);

    if(-1 == lstat(name, &buff))
    {
        perror("error(lstat)");
        return -1;
    }

    if(S_ISDIR(buff.st_mode))
    {
        if((strcmp(name, ".") == 0)
                || (strcmp(name, "..") == 0))
        {
            return 0;
        }

        DIR *pdir = NULL;

        if((pdir = opendir(name)) != NULL)
        {
            struct dirent *ent = NULL;

            while((ent = readdir(pdir)) != NULL)
            {
                if((strcmp(ent->d_name, ".") != 0)
                        && (strcmp(ent->d_name, "..") != 0))
                {
                    char newpath[BUFFSIZE] = {0};

                    strcpy(newpath, name);
                    strcat(newpath, "/");
                    strcat(newpath, ent->d_name);

                    //Delete new directory
                    if(-1 == _delete(newpath) )
                    {
                        cout << "error _delete" << endl;
                        cout << newpath << endl;
                        return -1;
                    }
                }
            }

            //Remove empty directory
            if(-1 ==  rmdir(name) )
            {
                perror("error rmdir:");
                cout << name << endl;
                return -1;
            }
        }
    }
    else
    {
        if( -1 == unlink(name))//delete file
        {
            perror("error(unlink)");
            return -1;
        }
    }

    return 0;
}

int client::rm()
{
    if(m_cmd.size() < 2)
    {
        cout << "missing oprand." << endl;
        return 0;
    }

    char file_dir[BUFFSIZE] = {0};
    strcpy(file_dir, m_cmd[1]);

    if( m_guider.is_at_local() )
    {
        return _delete(file_dir);
    }
    else
    {
        m_buff.clear();

        // Add the pack type.
        m_buff.push_front(PT_RM);

        // Add the oprand.
        add_buff(file_dir, strlen(file_dir) );

        // Add the checkcode.
        m_analyser.load_data(m_buff);
        m_buff.push_back(m_analyser.check());

        if( -1 == send_data() )
        {
            return -1;// Send error.
        }

        char pack_type = acceptable();
        if( is_pack_type(PT_NOENT, pack_type) )
        {
            cout << "file or directory is not exist"
                << endl;
        }
        else if( is_pack_type(PT_DOERR, pack_type) )
        {
            cout << "remove file or directory error"
                << endl;
        }
    }

    return 0;
}

int client::mkdir()
{
    int res = 0;

    if(m_cmd.size() < 2)
    {
        cout << "missing operand" << endl;
        return res;
    }

    char dir_name[BUFFSIZE] = {0};

    strcpy(dir_name, m_cmd[1]);

    if( m_guider.is_at_local() )
    {
        int old_mode = umask(0);

        if(-1 == (res = ::mkdir(dir_name, 0775)) )
        {
            cerr << "error mkdir in "
                << __FILE__ << " : "
                << __LINE__ << endl;
        }

        umask(old_mode);
    }
    else
    {
        m_buff.clear();

        // Add the pack type.
        m_buff.push_front(PT_MKDIR);

        // Add the new dir's name
        add_buff(dir_name, strlen(dir_name));

        // Add the checkcode.
        m_analyser.load_data(m_buff);
        m_buff.push_back(m_analyser.check());

        if(-1 != (res = send_data()) )
        {
            char pack_type = acceptable();
            if( is_pack_type(PT_DOERR, pack_type) )
            {
                cout << "error mkdir" << endl;
            }
            else if( is_pack_type(PT_EENT, pack_type) )
            {
                cout << "directory's name is exist"
                    << endl;
            }
        }
    }

    return res;
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
            res = rm();
            break;

        case CMD_MKDIR:
            res = mkdir();
            break;

        case CMD_PWD:
            pwd();
            break;

        case CMD_EXIT:
            over();
            break;

        case CMD_UPLD:
            res = upload();
            break;

        case CMD_DWLD:
            res = download();
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
