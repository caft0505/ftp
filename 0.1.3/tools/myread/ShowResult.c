#include "myread.h"

int ShowResult(Node *resultHead, const char *noShow,\
        int bSpace, char *cmd )
{
    Node *p = NULL;

    assert(NULL != resultHead);

    if(NULL == noShow)
    {
        if(! SortList(resultHead))
        {
            return -1;
        }

        ShowMore(resultHead);
    }
    else
    {
        p = resultHead->pnext;
        printf("%s", p->pdata + strlen(noShow));
        fflush(stdout);
        strcat(cmd, p->pdata + strlen(noShow));

        if(1 == bSpace)
        {
            write(STDOUT_FILENO, " ", 1);
            strcat(cmd, " ");
        }
    }

    return 0;
}
