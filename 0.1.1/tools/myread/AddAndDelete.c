#include "myread.h"

int Add(char *cmd, int pos, char nch)
{
    int add = 0;
    int len = 0;

    assert(NULL != cmd);

    len = strlen(cmd);

    if(len == pos)
    {
        write(STDOUT_FILENO, &nch, 1);

	cmd[pos] = nch;

	return 0;
    }

    write(STDOUT_FILENO, &nch, 1);

    write(STDOUT_FILENO, (cmd + pos), (len - pos));

    add = len - pos;

    while(add--)
    {
	printf("\b");
	fflush(stdout);
    }

    for(add = len; add > pos; --add)
    {
	cmd[add] = cmd[add-1];
    }

    cmd[pos] = nch;
    cmd[len + 1] = '\0';

    return 0;
}

int Delete(char *cmd, int pos)
{
    int del = 0;
    int len = 0;

    assert(NULL != cmd);

    len = strlen(cmd);

    printf("\b");
    fflush(stdout);

    write(STDOUT_FILENO, (cmd + pos), (len - pos));

    write(STDOUT_FILENO, " ", 1);

    del = len - pos + 1;

    while(del--)
    {
	printf("\b");
	fflush(stdout);
    }

    for(del = pos - 1; del < len; ++del)
    {
	cmd[del] = cmd[del+1];
    }

    cmd[len - 1] = '\0';

    return 0;
}
