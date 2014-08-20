#include "ls.h"

int ls(const char *argv[])
{
    int i 		= 1;
    int j 		= 1;
    int err 		= 0;
    int buffCount 	= 0;
    char style[4]   = {0};
    arg_type type 	= DEF;
    struct stat statbuf = {0};
    char *val 		= NULL;
    Node *head 		= NULL;
    const char *buff[BUFFMAX] = {NULL};
    //const char *buff[10] = {NULL};

    for( ;\
            (err < 1) && (argv[i] != NULL);\
            ++i)
    {
        //get '-a', '-l', '-al' or '-a -l'
        if('-' == argv[i][0])
        {
            for(j=1; argv[i][j]!='\0'; j++)
            {
                switch(argv[i][j])
                {
                    case 'a' :
                        type |= _A;
                        break;

                    case 'l' :
                        type |= _L;
                        break;

                    case 'h' :
                        type |= _H;
                        break;

                    default :
                        printf("Error '%s'\n", argv[i]);
                        err = 1;
                }
            }

        }
        else
        {
            //get the file or dir to 'ls'
            buff[buffCount++] = argv[i];
        }
    }

    // analysis 'll' is 'ls -l'
    if(0 == strcmp(argv[0], "ll") )
    {
        type |= _L;
    }

    //by default it scan current directory
    if((err < 1) && (NULL == buff[0]))
    {
        buff[0] = ".";

        buffCount = 1;
    }
    
    //initialize the list
    if(! InitList(&head))
    {
        return -1;
    }

    //'ls' all specified file or directory
    for(i = 0;\
            (err < 1) && (i < buffCount);\
            i++)
    {
        if(i > 0)
        {
            printf("\n");	
        }

        printf("%s :\n", buff[i]);

        if(-1 == lstat(buff[i], &statbuf))
        {
            perror("ERROR(lstat)");
            continue;
        }

        if(S_ISDIR(statbuf.st_mode))//show directory's information
        {
            if(! ShowDir(buff[i], type, head))
            {
                err = 1;
            }
        }
        else//show file's information
        {
            if(! ShowFile(buff[i], type,
                        statbuf, style, head))
            {
                err = 1;
            }
        }

        if(0 == err)
        {
            if(type & _L)
            {
                if(-1 == Show_L(head))
                {
                    err = 1;
                }
            }
            else
            {
                if(-1 == Show_NL(head))
                {
                    err = -1;
                }
            }
        }

        if(! CleanList(head))
        {
            err = 1;
        }
    }

    if(! DestroyList(&head))
    {
        err = -1;
    }

    return err;
}

//Copylist
int CopyList(Node *dest,
        const Node *source)
{
    const Node *p    = NULL;
    Node *q          = NULL;
    char *child      = NULL;
    char str[STRMAX] = {0};

    assert((NULL != dest)
            && (NULL != source));

    p = source->pnext;

    while(NULL != p)
    {
        strncpy(str, p->pdata, OFFS);

        if(NULL == (child = strchr(str, ' ')))
        {
            printf("ERROR(strchr) '%s'\n", str);
            return -1;
        }

        *child = '\0';

        if(! AddNode(dest, str))
        {
            return -1;
        }

        memset(str, 0, STRMAX);

        p = p->pnext;
    }

    return 0;
}

//Show not '-l' information
int Show_NL(Node *head)
{
    int i	            = 0;
    int j	            = 0;
    int k	            = 0;
    int row	            = 0;
    int col	            = 0;
    Style style         = {0};
    struct winsize size = {0};
    Node *pSlow         = NULL;
    Node *lpSlow        = NULL;
    Node *pFast         = NULL;
    Node *lpFast        = NULL;
    Node *localHead     = NULL;

    assert(NULL != head);

    if(! InitList(&localHead))
    {
        return -1;
    }

    if(-1 == GetTermSize(&size))
    {
        return -1;
    }

    if(! SortList(head))
    {
        return -1;
    }

    if(-1 == CopyList(localHead, head))
    {
        return -1;
    }

    style.row = size.ws_row;
    style.col = size.ws_col;

    if(-1 == GetShowStyle(localHead, &style))
    {
        return -1;
    }

    row = style.row;
    col = style.col;

    pSlow = head->pnext;
    lpSlow = localHead->pnext;

    //Print 'row' rows(k)
    //and 'col' cols(j)
    for(k = 0;
            (k < row) && (NULL != pSlow);
            ++k, pSlow = pSlow->pnext,
            lpSlow = lpSlow->pnext)
    {
        pFast = pSlow;
        lpFast = lpSlow;

        //Every row has 'col' cols.
        for(j = 0; j < col; ++j)
        {
            if(NULL != pFast)
            {
                printf("%s", pFast->pdata + OFFS);
                printf("%*c",
                        style.blank[j] - strlen(lpFast->pdata),
                        ' ');
            }

            //Get next data to print.
            for(i = 0;
                    (i < row) && (NULL != pFast);
                    ++i, pFast = pFast->pnext,
                    lpFast = lpFast->pnext)
            {
                NULL;
            }
        }

        puts("");
    }

    return 0;
}

int Show_L(Node *head)
{
    Node *p = NULL;

    assert(NULL != head);

    if( ! SortList(head))
    {
        return -1;
    }

    p = head->pnext;

    while(NULL != p)
    {
        //Have to add OFFSET to p->pdata 
        //because the file name appear at
        //the p->pdata's head for sort
        //and it also appear at another
        //place.
        printf("%s", (p->pdata + OFFSET));
        fflush(stdout);
        printf("\n");
        fflush(stdout);

        p = p->pnext;
    }

    return 0;
}

bool ShowFile(const char *file, arg_type type,\
        struct stat statbuf, char style[], Node *head)
{
    char tmpStore[BUFFMAX] = {0};

    assert((NULL != file)
            && (NULL != head)
            && (NULL != style));

    if(0 == (style[0]+style[1]+style[2]+style[3]))
    {
        if(-1 == GetLength(statbuf, type, style))
        {
            return -1;
        }
    }

    sprintf(tmpStore, "%-50s", file);

    if(type & _L)//                 if mark '-l'
    {
        if(S_ISFIFO(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"f");

            Print_L(statbuf, type, style, tmpStore);
        }
        else if(S_ISCHR(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"c");

            Print_L(statbuf, type, style, tmpStore);
        }
        else if(S_ISBLK(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"b");

            Print_L(statbuf, type, style, tmpStore);
        }
        else if(S_ISLNK(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"l");

            Print_L(statbuf, type, style, tmpStore);
        }
        else if(S_ISSOCK(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"s");

            Print_L(statbuf, type, style, tmpStore);
        }
        else if(IS_S(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"-");

            Print_L(statbuf, type, style, tmpStore);
        }
        else if(IS_X(statbuf.st_mode))
        {
            sprintf(tmpStore+strlen(tmpStore),"-");

            Print_L(statbuf, type, style, tmpStore);
        }
        else
        {
            sprintf(tmpStore+strlen(tmpStore),"-");

            Print_L(statbuf, type, style, tmpStore);
        }
    }

    //Get the file's name(not care about whether mark '-l')
    if(S_ISFIFO(statbuf.st_mode))
    {
        sprintf(tmpStore+strlen(tmpStore),\
                "\33[1;33m%s\33[0m", file);
    }
    else if(IS_S(statbuf.st_mode))//s_uid or s_gid is seted
    {
        sprintf(tmpStore+strlen(tmpStore),\
                "\33[1;41;37m%s\33[0m", file);
    }
    else if(IS_X(statbuf.st_mode))//the file can be run
    {
        sprintf(tmpStore+strlen(tmpStore),\
                "\33[1;32m%s\33[0m", file);   
    }
    else
    {
        sprintf(tmpStore+strlen(tmpStore),\
                "%s", file);
    }

    return AddNode(head, tmpStore);
}

void Print_L(struct stat statbuf,arg_type type,
        char style[], char tmpStore[])
{
    int mode 		  = 0;
    int unit_i 		  = 0;
    float size 		  = 0.0f;
    struct passwd *pwu 	  = getpwuid(statbuf.st_uid);
    struct group *grp 	  = getgrgid(statbuf.st_gid);
    struct tm *timeCreate = NULL;
    char *timeCreateStr   = NULL;
    const char unit[] 	  = {' ', 'K', 'M', 'G', 'T'};
    char tmp[BUFFMAX]     = {0};

    assert(NULL != tmpStore);

    //the user's limits of authority
    sprintf(tmpStore+strlen(tmpStore),\
            IS_UR(statbuf.st_mode) ? "r" : "-");//user read
    sprintf(tmpStore+strlen(tmpStore),\
            IS_UW(statbuf.st_mode) ? "w" : "-");//user write
    if(IS_SU(statbuf.st_mode))
    {
        sprintf(tmpStore+strlen(tmpStore),\
                IS_LOWER_SU(statbuf.st_mode) ? "s" : "S");//set s_uid
    }
    else
    {
        sprintf(tmpStore+strlen(tmpStore),\
                IS_UX(statbuf.st_mode) ? "x" : "-");//user execute
    }

    //the group's limits of authority
    sprintf(tmpStore+strlen(tmpStore),\
            IS_GR(statbuf.st_mode) ? "r" : "-");//group read
    sprintf(tmpStore+strlen(tmpStore),\
            IS_GW(statbuf.st_mode) ? "w" : "-");//group write
    if(IS_SG(statbuf.st_mode))
    {
        sprintf(tmpStore+strlen(tmpStore),\
                IS_LOWER_SG(statbuf.st_mode) ? "s" : "S");//set s_gid
    }
    else
    {
        sprintf(tmpStore+strlen(tmpStore),\
                IS_GX(statbuf.st_mode) ? "x" : "-");//group execute
    }


    //the others' limits of authority
    sprintf(tmpStore+strlen(tmpStore),\
            IS_OR(statbuf.st_mode) ? "r" : "-");//others read
    sprintf(tmpStore+strlen(tmpStore),\
            IS_OW(statbuf.st_mode) ? "w" : "-");//others write
    if(IS_OT(statbuf.st_mode))
    {
        sprintf(tmpStore+strlen(tmpStore),\
                IS_LOWER_OT(statbuf.st_mode) ? "t" : "T");//set o_vtx
    }
    else
    {
        sprintf(tmpStore+strlen(tmpStore),\
                IS_OX(statbuf.st_mode) ? "x" : "-");//others execute
    }

    //the file or directory's link-numbers
    sprintf(tmpStore+strlen(tmpStore),\
            ". %*d ", style[0], statbuf.st_nlink);

    //the username and group-name of owner
    sprintf(tmpStore+strlen(tmpStore),\
            "%-*s %-*s ", style[1], pwu->pw_name,
            style[2], grp->gr_name);

    //the file or directory's size
    if(type & _H)
    {
        size = statbuf.st_size;

        //get the appropriate unit to show
        while((size / ONE_K) >= 1)
        {
            ++unit_i;
            size = statbuf.st_size / Pow(ONE_K, unit_i);
        }

        //Formart as same as system show.
        //If (size-(int)size) < PRECISION 
        //size's 'value' is a integer. For
        //others, add 1 to it's percent-bit
        //when print size by formart string
        //"%.1f" tail number not carry-over.
        if(((size - (int)size) > PRECISION) &&\
                ((((int)(size * 100)) % 10) < 5))
        {
            size += 0.1;
        }

        //To let it flush right even if it hasn't unit.
        if(0 == unit_i)
        {
            sprintf(tmp, "%*d", style[3], statbuf.st_size);
        }
        else
        {
            sprintf(tmp, "%*.1f%c", style[3],
                    size, unit[unit_i]);
        }

        //Add one space to hold 'K', 'M' and so on.
        sprintf(tmpStore+strlen(tmpStore),\
                "%*s ", style[3]+1, tmp);
    }
    else
    {
        sprintf(tmpStore+strlen(tmpStore),\
                "%*d ", style[3], statbuf.st_size);
    }

    //get the last time of change status
    timeCreate = localtime(&statbuf.st_ctime);

    timeCreateStr = asctime(timeCreate);
    timeCreateStr[strlen(timeCreateStr)-9] = '\0';
    sprintf(tmpStore+strlen(tmpStore),\
            "%s ", (timeCreateStr+4) );
}

float Pow(float sdata, int exponent)
{
    if(exponent < 1)
    {
        return 1;
    }

    return (sdata * Pow(sdata, --exponent));
}

int GetLength(struct stat statbuf, arg_type type, char maxStyle[])
{
    int i                 = 0;
    float size 		      = 0.0f;
    char tmp[BUFFMAX]     = {0};
    struct passwd *pwu 	  = NULL;
    struct group *grp 	  = NULL;

    assert(NULL != maxStyle);

    //Get maxlength of file-linknum
    sprintf(tmp, "%d", statbuf.st_nlink);
    if(strlen(tmp) > maxStyle[0])
    {
        maxStyle[0] = strlen(tmp);
    }

    //Get maxlength of file-owner
    if(NULL == (pwu = getpwuid(statbuf.st_uid)))
    {
        perror("ERROR(getpwuid)");
        return -1;
    }
    if(strlen(pwu->pw_name) > maxStyle[1])
    {
        maxStyle[1] = strlen(pwu->pw_name);
    }

    //Get maxlength of file-ownergroup
    if(NULL == (grp = getgrgid(statbuf.st_gid)))
    {
        perror("ERROR(getgrgid)");
        return -1;
    }
    if(strlen(grp->gr_name) > maxStyle[2])
    {
        maxStyle[2] = strlen(grp->gr_name);
    }

    //Get maxlength of the file-size
    if(type & _H)
    {
        size = statbuf.st_size;

        //get the appropriate unit to show
        while((size / ONE_K) >= 1)
        {
            ++i;
            size = statbuf.st_size / Pow(ONE_K, i);
        }

        //Formart as same as system show.
        //If (size-(int)size) < PRECISION 
        //size's 'value' is a integer. For
        //others, add 1 to it's percent-bit
        //when print size by formart string
        //"%.1f" tail number not carry-over.
        if(((size - (int)size) > PRECISION) &&\
                ((((int)(size * 100)) % 10) < 5))
        {
            size += 0.1;
        }

        sprintf(tmp,\
                (0 == i) ?\
                "%.0f" : "%.1f",\
                size);
    }
    else
    {
        sprintf(tmp,\
                "%d", statbuf.st_size);
    }
    if(strlen(tmp) > maxStyle[3])
    {
        maxStyle[3] = strlen(tmp);
    }

    return 0;
}

int GetLStyle(const char *filePath, arg_type type, char style[])
{
    struct dirent *entry  = NULL;
    DIR *dp               = NULL;
    struct stat statbuf   = {0};
    char maxStyle[4]      = {0};
    char oldPath[BUFFMAX] = {0};

    assert((NULL != filePath)
            && (NULL != style));

    if(NULL == (dp = opendir(filePath)))
    {
        printf("ERROR(opendir) '%s': ", filePath);
        perror("");
        return -1;
    }

    if(NULL == getcwd(oldPath, BUFFMAX-1))
    {
        perror("ERROR(getcwd)");
        return false;
    }

    if(-1 == chdir(filePath))
    {
        printf("ERROR(chdir) '%s' ", filePath);
        fflush(stdout);
        perror("");
        return false;
    }

    while(NULL != (entry = readdir(dp)))
    {
        if(-1 == lstat(entry->d_name, &statbuf))
        {
            printf("ERROR(lstat) '%s': ", entry->d_name);
            fflush(stdout);
            perror("");
            closedir(dp);
            return -1;
        }

        if(! (type & _A) && ('.' == entry->d_name[0]))
        {
            continue;
        }

        if(-1 == GetLength(statbuf, type, maxStyle))
        {
            closedir(dp);
            return -1;
        }
    }

    closedir(dp);

    if(-1 == chdir(oldPath))
    {
        perror("ERROR(chdir)");
        return false;
    }

    int i = 0;
    for( ; i < 4; ++i)
    {
        style[i] = maxStyle[i];
    }

    return 0;
}

bool ShowDir(const char *dir, arg_type type, Node *head)
{
    DIR *dp			= NULL;
    struct dirent *entry	= NULL;
    struct stat statbuf		= {0};
    char oldPath[BUFFMAX] 	= {0};
    char tmpStore[BUFFMAX] 	= {0};
    char style[4]           = {0};
    int sizeOfDir 		= 0;
    float size 			= 0.0f;
    int unit_i 			= 0;
    const char unit[] 		= {'K', 'M', 'G', 'T'};

    assert((NULL != dir)
            && (NULL != head));

    //Open the directory to show
    if(NULL == (dp = opendir(dir)))
    {
        printf("ERROR(opendir) '%s': ", dir);
        fflush(stdout);
        perror("");
        return false;
    }

    //If mark '-l', Get length of the file-linknum,
    //file-size, fileowner,fileownergroup
    if(type & _L)
    {
        if(-1 == GetLStyle(dir, type, style))
        {
            return -1;
        }
    }

    if(NULL == getcwd(oldPath, BUFFMAX-1))
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

        //If not mark '-a' filter '.', '..' and '.*'
        if(!(type & _A)
                && ( '.' == entry->d_name[0] ) )
        {
            continue;
        }

        //Count the directory's size
        sizeOfDir += statbuf.st_blocks/2;

        //Show directory's information
        if(S_ISDIR(statbuf.st_mode))
        {
            //Get file name for sort
            sprintf(tmpStore, "%-50s", entry->d_name);

            if(type & _L)//                    mark '-l'
            {
                sprintf(tmpStore+strlen(tmpStore), "d");
                Print_L(statbuf, type, style, tmpStore);
            }

            sprintf(tmpStore+strlen(tmpStore),\
                    "\33[1;34m%s\33[0m", entry->d_name);

            //Add the new node(:'tmpStore') including
            //the new file or directory's information
            if(! AddNode(head, tmpStore))
            {
                return false;
            }

            memset(tmpStore, '\0', BUFFMAX);
        }
        else//                Show file's information
        {
            ShowFile(entry->d_name, type,
                    statbuf, style, head);
        }
    }

    //Change position into old path
    if(-1 == chdir(oldPath))
    {
        perror("ERROR(chdir)");
        return false;
    }

    closedir(dp);

    if(type & _L)//    If mark '-l'
    {
        printf("total ");

        if(type & _H)//If mark '-h'
        {
            size = sizeOfDir;

            //Get the appropriate unit to show
            while((size / ONE_K) >= 1)
            {
                ++unit_i;
                size = statbuf.st_size / Pow(ONE_K, unit_i);
            }

            //To formart as same as system
            if(((size - (int)size) > PRECISION) &&\
                    ((((int)size * 100) % 10) < 5))
            {
                size += 0.1;
            }

            printf((0 == unit_i) ?\
                    "%.0f%c " : "%.1f%c ",\
                    size, unit[unit_i]);
        }
        else//         If not mark '-h'
        {
            printf("%d ", sizeOfDir);
        }

        printf("\n");
    }

    return true;
}
