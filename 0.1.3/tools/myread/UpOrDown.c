#include "myread.h"

int UpOrDown(Node *cmdHead, int cursorPos,
        int nodePos, char *lastBuf,
        char *cmd)
{
    int i	= 0;
    int listLen = 0;
    int cmdLen  = 0;
    char *val	= NULL;

    assert(NULL != cmdHead);

    if((listLen = GetLen(cmdHead)) < 1)
    {
        return 0;
    }

    if((cmdLen = strlen(cmd)) > 0)
    {
        for(i = cursorPos; i < cmdLen; ++i)
        {
            write(STDOUT_FILENO, " ", 1);
        }

        for(i = 0; i< cmdLen; ++i)
        {
            printf("\b \b");
            fflush(stdout);
        }
    }

    memset(cmd, 0, cmdLen);

    if(0 == nodePos)
    {
        write(STDOUT_FILENO, lastBuf, strlen(lastBuf));
        strcpy(cmd, lastBuf);
    }
    if(NULL != (val = GetVal(cmdHead, (listLen - nodePos + 1))))
    {
        write(STDOUT_FILENO, val, strlen(val));
        strcpy(cmd, val);
    }

    return 1;
}
