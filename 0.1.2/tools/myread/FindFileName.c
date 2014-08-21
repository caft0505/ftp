#include "myread.h"

int GetIndexOfSameWord(const char *dest,\
		     const char *source)
{
    int index = -1;

    while(*dest == *source)
    {
	++index;
	++dest;
	++source;
    }

    return index;
}

int FindFileName(Node *resultHead, const char *path,\
		const char *childStr)
{
    int i		   = 0;
    int min		   = 0;
    int index		   = -1;
    int strLen		   = 0;
    int listLen		   = 0;
    char name[BUFFSIZE]    = {0};
    char newPath[BUFFSIZE] = {0};
    struct stat buf	   = {0};
    DIR *dp		   = NULL;
    struct dirent *entry   = NULL;

    assert(NULL != resultHead);
    assert(NULL != path);
    assert(NULL != childStr);

    if(NULL == (dp = opendir(path)))
    {
//	printf("ERROR(opendir) '%s': ", path);
//	fflush(stdout);
//	perror("");
	return -1;
    }

    strLen = strlen(childStr);

    while(NULL != (entry = readdir(dp)))
    {
	if(! strcmp(entry->d_name, ".") ||\
	   ! strcmp(entry->d_name, ".."))
	{
	    continue;
	}

	if( ('\0' == *childStr) ||
	((strLen-1) <= GetIndexOfSameWord(
		 	entry->d_name,
			    childStr )) )
	{
	    strcpy(newPath, path);
	    if('/' != newPath[strlen(newPath) - 1])
	    {
		strcat(newPath, "/");
	    }
	    strcat(newPath, entry->d_name);

	    if(-1 == lstat(newPath, &buf))
	    {
		printf("ERROR(lstat) '%s' ", newPath);
	  	fflush(stdout);
		perror("");
		return -1;
	    }

	    strcpy(name, entry->d_name);

	    if(S_ISDIR(buf.st_mode))
	    {
		strcat(name, "/");
	    }

	    if(! AddNode(resultHead, name))
	    {
	        return -1;
	    }

	    memset(name, '\0', BUFFSIZE);
	    memset(newPath, '\0', BUFFSIZE);
	}
    }


    //If childStr is not "\0" and find
    //childStr's parent more than one,
    //leave the most possible 'parent'
    //(get the same word as far as possible).
    if( ('\0' != *childStr)
     && ( (listLen = GetLen(resultHead)) > 1 ) )
    {
	for( i = 2; i <= listLen; ++i )
	{
	    index = GetIndexOfSameWord(
		    GetVal(resultHead, i-1),
		      GetVal(resultHead, i) );

	    if(2 == i)
	    {
		min = index;
	    }

	    min = ((min > index) ? index : min);
	}

	strcpy(name, GetVal(resultHead, 1));
	name[min + 1] = '\0';

	if( strcmp(childStr, name)
	 && ( (! CleanList(resultHead))
	   || (! AddNode(resultHead, name)) ) )
	{
	    return -1;
	}

	return 1;
    }

    return 0;
}
