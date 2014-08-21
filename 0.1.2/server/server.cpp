#include "server.h"

int server::deal_cmd()
{
    char buff[BUFFSIZE] = {0};

    int res = recv(m_fd, buff, BUFFSIZE, 0);
    if(res <= 0)
    {
        return -1;
    }

    set_buff(buff, res);

    m_analyser.load_data(buff, res);

    if(m_analyser.checkout())
    {
        reply(PT_OK);

        // Delete the checkcode.
        m_buff.pop_back();
    }
    else
    {
        reply(PT_ERROR); // Error package.
    }

    return 0;
}

int server::dispatch()
{
    int res = 0;
    char pack_type = m_analyser.get_pack_type();

    // Delete the pack type.
    m_buff.pop_front();

    switch(pack_type)
    {
        case PT_UPREQ:
            break;
        
        case PT_DWREQ:
            break;

        case PT_LS:
            res = ls();
            break;

        case PT_CD:
            cd();
            break;

        case PT_RM:
            break;
        
        case PT_PWD:
            pwd();
            break;
        
        case PT_EXIT:
            over();
            break;
        
        default:
            break;
    }
    
    return res;
}

int server::ls()
{
    DIR *dp			= NULL;
    struct dirent *entry	= NULL;
    struct stat statbuf		= {0};
    char buff[BUFFSIZE]     = {0};
    char oldPath[BUFFSIZE] 	= {0};
    char dir[BUFFSIZE] = {0};

    get_buff(dir, BUFFSIZE);

    //Open the directory to show
    if(NULL == (dp = opendir(dir)))
    {
        printf("ERROR(opendir) '%s': ", dir);
        fflush(stdout);
        perror("");
        return false;
    }

    if(NULL == getcwd(oldPath, BUFFSIZE))
    {
        perror("ERROR(getcwd)");
        return false;
    }

    //Change current position into specified directory
    if(-1 == chdir(dir))
    {
        printf("ERROR(chdir) '%s' ", dir);
        fflush(stdout);
        perror("");
        return false;
    }

    //Get next file or dir's entry
    while((entry = readdir(dp)) != NULL)
    {
        //Get the file or directory's stat
        if(-1 == lstat(entry->d_name, &statbuf))
        {
            printf("ERROR(lstat) '%s' ",
                            entry->d_name);
            fflush(stdout);
            perror("");
            closedir(dp);
            return false;
        }

        //Filter '.', '..' and '.*'
        if( '.' == entry->d_name[0] )
        {
            continue;
        }

        strcat(buff, entry->d_name);

        if( S_ISDIR(statbuf.st_mode) )
        {
            strcat(buff, "/");
        }
        else if( statbuf.st_mode & 0111)
        {
            strcat(buff, "*");
        }

        strcat(buff, "  ");

    }

    //Change position into old path
    if(-1 == chdir(oldPath))
    {
        perror("ERROR(chdir)");
        return false;
    }

    closedir(dp);

    // If no file or directory in the path,
    // add blankspace into buff to translate.
    if('\0' == buff[0])
    {
        buff[0] = ' ';
    }

    // Init the m_buff
    set_buff(buff, strlen(buff));
    bzero(buff, BUFFSIZE);

    // Add the data size.
    parse::parse_str(buff, sizeof(short), m_buff.size());
    m_buff.insert(m_buff.begin(), buff, buff + sizeof(short));

    // Add the pack type.
    m_buff.push_front(PT_DATA);

    m_analyser.load_data(m_buff);
    // Add the pack checkcode.
    m_buff.push_back(m_analyser.check());

    if(-1 == send_data() )
    {
        ;
    }

    return 0;
}

void server::cd()
{
    if(m_guider.is_at_local() )
    {
        char path[BUFFSIZE] = {0};

        get_buff(path, BUFFSIZE);

        if( -1 == m_guider.cd(path) )
        {
            reply(PT_NOENT);
        }
        else
        {
            reply(PT_OK);
        }
    }
}

void server::pwd()
{
    char path[BUFFSIZE] = {0};

    if(NULL == m_guider.pwd(path, BUFFSIZE))
    {
        ;// Error .
        strcpy(path, "unknow path");
    }

    // Add the pack type.
    m_buff.push_front(PT_DATA);

    // Add the path size.
    char tmp[2] = {0};
    parse::parse_str(tmp, 2, strlen(path));
    m_buff.insert(m_buff.begin() + 1, tmp, tmp + 2);

    // Add the path.
    add_buff(path, strlen(path));

    m_analyser.load_data(m_buff);

    // Add the checkcode.
    m_buff.push_back(m_analyser.check());

    if(-1 == send_data() )
    {
        ;// Error.
    }
    
}
