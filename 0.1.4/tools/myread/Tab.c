#include "myread.h"

const char * GetTail(const char *cmd, char token)
{
    int i			= 0;
    int pos			= -1;
    const char *tmp	      	= NULL;
    static char ret[BUFFSIZE] 	= {0};

    assert(NULL != cmd);

    memset(ret, '\0', BUFFSIZE);

    tmp = cmd;

    while('\0' != *tmp)
    {
	if(token == *tmp)
	{
	    pos = i;
	}

	++i;
	++tmp;
    }

    return (strcpy(ret, cmd + pos + 1));
}

int Tab(Node *resultHead, const char *info,\
	    const char *path, int ti,\
			   char *cmd)
{
    int mode		 = 0;
    int i		 = 0;
    int len		 = 0;
    int times		 = 2;
    int flag 		 = -1;
    int pos		 = -1;
    char tmp[BUFFSIZE]	 = {0};
    struct stat buf	 = {0};
    char child[BUFFSIZE] = {0};
    const char *s	 = NULL;

    assert(NULL != resultHead);
    assert(NULL != path);

    strcpy(tmp, path);

    if('\0' == *tmp)
    {
	strcpy(tmp, ".");
    }

    //The path is exist or not exist?
    while((times--)\
	&& (-1 == (flag = lstat(tmp, &buf))))
    {
	if(ENOENT != errno)
	{
	    return -1;
	}

	if(times > 0)
	{
	    s = GetTail(tmp, '/');

	    //If string tmp is not equal to it's
	    //tail string s(is a part of string tmp
	    //which contain the last '/' int tmp),
	    //delete the tail(string s) of string
	    //tmp (maybe the new string tmp[the old
	    //string tmp delete tail s] is exist).
	    //But if string tmp is equal to string s
	    //means the string tmp(as same as string
	    // path) doesn't contain a '/'(break).
	    if(strcmp(tmp, s))
	    {
	        tmp[strlen(tmp) - strlen(s)] = '\0';
	    }
	    else
	    {
		break;
	    }
	}
    }

    //Get string child from the tail of
    //string path.
    strcpy(child, GetTail(path, '/'));

    //If 'times = 1', means the path isn't
    //contain '/' or the path is exist.
    /////And in this section string path is
    //equal to string tmp.
    //Else if 'times = -1', means the string
    //tmp is a part of string path. Maybe tmp
    //is exist or not exist.
    if(1 == times)
    {
	//If 'flag = -1', maybe the path is
	//a part of file name in current
	//directory.
	//Else if 'flag = 0', means the path
	//is exist.
	if(-1 == flag)
	{
	    strcpy(tmp, ".");
	}
	else
	{
	    if(! S_ISDIR(buf.st_mode))
	    {
		write(STDOUT_FILENO, " ", 1);
	    	strcat(cmd, " ");
	    }
	    else
	    {
		if( ('\0' != path[0])
		  && ('/'  != path[strlen(path) - 1])
		  && (('.' != path[strlen(path) - 1])
		  || ('.') == path[strlen(path) - 2]))
		{
		    write(STDOUT_FILENO, "/", 1);
		    strcat(cmd, "/");
		}
	    }

	    s = GetTail(tmp, '/');

	    if( strcmp(path, ".")
	      && ( ('.' != path[strlen(path) - 1])
		    || ('.') == path[strlen(path) - 2]))
	    {
	        //Because path is exist so click button
	        //'Tab' means to get other file name of
	        //the specified directory(name of 'path'
	        //is also included in this directory).
	        strcpy(child, "\0");
	    }

	    //If path is not a directory.
	    //If string path isn't equal to string
	    //s(is tail of string path), means the
	    //s(file) is exist in 'path-s'.
	    //Else string path is a file exist in
	    //current directory.
	    if(! S_ISDIR(buf.st_mode))
	    {
	        if(0 != strcmp(tmp, s))
	        {
		    tmp[strlen(path) - strlen(s)] = '\0';
	        }
	        else
	        {
		    strcpy(tmp, ".");
	        }
	    }
	}
    }
    else
    {
	//If 'flag = -1', the path and tmp
	//is not exist.
	//Else the tmp(delete the tail of path)
	//is exist.
	if(-1 == flag)
	{
	    return -1;
	}
    }

    if( -1 == (flag = FindFileName(resultHead,
			    tmp, child)) )
    {
	return -1;
    }

    if((len = GetLen(resultHead)) > 0)
    {
        if(len > 1)
        {
	    if(ti > 0)
	    {
	        if(-1 ==  ShowResult(resultHead, NULL,\
					1, cmd))
	        {
		    return -1;
	        }

		write(STDOUT_FILENO, info, strlen(info));

		write(STDOUT_FILENO, cmd, strlen(cmd));
	    }
        }
        else
        {
	    strcpy(tmp, GetVal(resultHead, 1));

	    flag = ( flag ? 0 :
		   ('/' != tmp[strlen(tmp) - 1]) );

	    if( -1 ==  ShowResult(resultHead, child,
				    flag, cmd) )
	    {
	        return -1;
	    }
        }
    }

    return 0;
}
