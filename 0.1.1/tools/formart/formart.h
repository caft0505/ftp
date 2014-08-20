#ifndef _MYSHELL_TOOLS_FORMART_FORMART_H_
#define _MYSHELL_TOOLS_FORMART_FORMART_H_


#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <assert.h>
#include <string.h>
#include <sys/ioctl.h>

#include "../list/list.h"

#define STRMAX 512
#define OFFS    50

typedef struct
{
    int row;
    int col;
    int blank[50];
}Style;

typedef struct
{
    int sumlen;
    int maxlen;
}Len;

//Get the current terminal's size(rows
//and cols).
//Output: size
int GetTermSize(struct winsize *size);

//Get the sumlen and the maxlen of
//node's data length from cur to
//'cur+num'.
//Input: head
//Output: len
int GetNodeLen(Node *cur, int num, Len *len);

//Get the style of print to show list,
//style->ws_row rows and style->ws_col
//cols.
//Input: head, style
//Output: style
int GetShowStyle(const Node *head,
                 Style *style);

#endif
