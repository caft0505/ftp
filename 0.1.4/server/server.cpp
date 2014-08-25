#include "server.h"

int server::deal_cmd()
{
    return recv_data();
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
            res = deal_upload();
            break;
        
        case PT_DWREQ:
            res = deal_download();
            break;

        case PT_LS:
            res = ls();
            break;

        case PT_CD:
            cd();
            break;

        case PT_RM:
            rm();
            break;
        
        case PT_MKDIR:
            mkdir();
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

//Delete dir or others in this 'name'
//
//Input: name
int server::_delete(const char *name)
{
    if(NULL == name)
    {
        return 0;
    }
    
    struct stat buff = {0};

    assert(NULL != name);

    if(-1 == lstat(name, &buff))
    {
        perror("ERROR(lstat)");
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
        if( -1 == unlink(name))//Delete file
        {
            perror("ERROR(unlink)");
            return -1;
        }
    }

    return 0;
}

int server::rm()
{
    // Get the file or directory name.
    char file_dir[BUFFSIZE] = {0};
    get_buff(file_dir, BUFFSIZE);

    // File or directory is exist.
    if( 0 == access(file_dir, F_OK) )
    {
        // Delete success.
        if( 0 == _delete(file_dir) )
        {
            reply(PT_OK);

            return 0;
        }

        // Delete failed.
        reply(PT_DOERR);
        return -1;
    }

    // File or directory is not exist.
    reply(PT_NOENT);

    return 0;
}

int server::mkdir()
{
    int res = 0;

    char new_dir[BUFFSIZE] = {0};

    get_buff(new_dir, BUFFSIZE);

    if( 0 == access(new_dir, F_OK) )
    {
        reply(PT_EENT);
        return res;
    }
    else
    {
        int old_mode = umask(0);

        if(-1 == (res = ::mkdir(new_dir, 0775)) )
        {
            cerr << "error mkdir in "
                << __FILE__ << " : "
                << __LINE__ << endl;

            reply(PT_DOERR);
        }
        else
        {
            reply(PT_OK);
        }

        umask(old_mode);
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

    // Get directory information to show
    // from package.
    get_buff(dir, BUFFSIZE);

    //Open the directory to show
    if(NULL == (dp = opendir(dir)))
    {
        printf("error(opendir) '%s': ", dir);
        fflush(stdout);
        perror("");
        return -1;
    }

    if(NULL == getcwd(oldPath, BUFFSIZE))
    {
        perror("error(getcwd)");
        return -1;
    }

    //Change current position into specified directory
    if(-1 == chdir(dir))
    {
        printf("error(chdir) '%s' ", dir);
        fflush(stdout);
        perror("");
        return -1;
    }

    //Get next file or dir's entry
    while((entry = readdir(dp)) != NULL)
    {
        //Get the file or directory's stat
        if(-1 == lstat(entry->d_name, &statbuf))
        {
            printf("error(lstat) '%s' ",
                            entry->d_name);
            fflush(stdout);
            perror("");
            closedir(dp);
            return -1;
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
        else if( statbuf.st_mode & 0111)// 0111: exe
        {
            strcat(buff, "*");
        }

        strcat(buff, "  ");

    }

    //Change position into old path
    if(-1 == chdir(oldPath))
    {
        perror("error(chdir)");
        return -1;
    }

    closedir(dp);

    // If no file or directory in the path,
    // add blankspace into buff to translate.
    if('\0' == buff[0])
    {
        buff[0] = ' ';
    }

    m_buff.clear();

    // Add the pack type.
    m_buff.push_front(PT_DATA);

    // Add the information.
    add_buff(buff, strlen(buff));

    // Add the pack checkcode.
    m_analyser.load_data(m_buff);
    m_buff.push_back(m_analyser.check());

    if(-1 == send_data() )
    {
        return -1;
    }

    return 0;
}

void server::cd()
{
    // Must be at local.
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

    m_buff.clear();

    // Add the pack type.
    m_buff.push_front(PT_DATA);

    // Add the path.
    add_buff(path, strlen(path));

    // Add the checkcode.
    m_analyser.load_data(m_buff);
    m_buff.push_back(m_analyser.check());

    if(-1 == send_data() )
    {
        cerr << "Send error in "
            << __FILE__ << " : "
            << __LINE__ << endl;
    }
}

int server::deal_upload()
{
    char file_name[BUFFSIZE] = {0};

    m_analyser.get_file_name(file_name, BUFFSIZE);

    if(0 == access(file_name, F_OK) )
    {
        reply(PT_FEXIST);

        char pack_type = acceptable();
        if( is_pack_type(PT_NOCOVER, pack_type) )
        {
            return 0;
        }
        else
        {
            ;// Cover the exist file.
        }
    }
    else
    {
        reply(PT_OK);
    }

    return recv_file();
}

int server::deal_download()
{
    char file_name[BUFFSIZE] = {0};
    get_buff(file_name, BUFFSIZE);

    if( -1 == access(file_name, F_OK) )
    {
        reply(PT_FNOEXIST);
        return 0;
    }
    else
    {
        reply(PT_OK);

        send_file_info(file_name);

        return send_file(file_name);
    }
}
