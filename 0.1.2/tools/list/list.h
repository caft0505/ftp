#ifndef	_MYSHELL_TOOLS_LIST_LIST_H_
#define	_MYSHELL_TOOLS_LIST_LIST_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <termios.h>

#define EQUAL	0
#define SMALLER	-1
#define BIGER	1

#define NEWLINE	100
#define VALMAX	512

typedef struct node
{
    char *pdata;//Save the value,which type is char *
    struct node *pnext;
}Node;

bool InitList(Node **phead);//Initialize the list

//Add node with the new-value into the list
bool AddNode(Node *head, char *newval);

//Delete the node accoding to specified index
char * DelNode(Node *head, int index);

//Get the value of the list with specific index
char * GetVal(const Node *head, int index);

int GetLen(const Node *head);//Get length of the list

//Compare destination string and source string,
//but ignore the word is upper or lower.
int my_strcmp(const char *dest, const char *source);

//Sort the list according to the order appears
//in the alphabet, but don's care about upper
//or lower.
bool SortList(Node *head);

bool CleanList(Node *head);//Clean the list
bool DestroyList(Node **phead);//Destroy the list

#endif
