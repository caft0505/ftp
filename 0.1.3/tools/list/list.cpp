#include "list.h"

bool InitList(Node **phead)
{
    *phead = (Node *)malloc(sizeof(Node));

    if(*phead == NULL)
    {
        printf("ERROR(malloc)\n");
        return false;
    }

    (*phead)->pdata = NULL;
    (*phead)->pnext = NULL;

    return true;
}

bool AddNode(Node *head, char *newval)
{
    Node * TempNode = NULL;

    assert((NULL != head)
            && (NULL != newval));

    TempNode = (Node *)malloc( sizeof(Node) );
    if(NULL == TempNode)
    {
        printf("ERROR(malloc)\n");
        return false;
    }

    TempNode->pnext = NULL;
    TempNode->pdata = (char *)malloc( strlen(newval)+1 );
    if(NULL == TempNode->pdata)
    {
        printf("ERROR(malloc)\n");
        return false;
    }

    strcpy(TempNode->pdata, newval);

    Node *p = head;
    while(p->pnext != NULL)
    {
        p = p->pnext;
    }

    p->pnext = TempNode;

    return true;
}

char * DelNode(Node *head, int index)
{
    int ind		    = 1;
    Node *current 	    = NULL;
    Node *next   	    = NULL;
    Node *prior 	    = NULL;
    static char val[VALMAX] = {0};

    assert(NULL != head);

    prior   = head;
    current = head->pnext;

    while( (NULL != current)
            &&   (ind < index) )
    {
        ++ind;

        prior   = current;
        current = current->pnext;
    }

    if(ind < index)
    {
        printf("ERROR(DelNode): "
                "No node index is %d\n", index);

        return NULL;
    }

    prior->pnext = current->pnext;

    strcpy(val, current->pdata);

    free(current->pdata);
    free(current);

    return val;
}

char * GetVal(const Node *head, int index)
{
    Node *p = NULL;

    assert(NULL != head);

    /*    if( (index < 1)
          || (index > GetLen(head)) )
          {
          return NULL;
          }
          */
    p = head->pnext;

    while(--index && (p!=NULL))
    {
        p = p->pnext;
    }

    return ( (p!=NULL) ? p->pdata : NULL);
}

int GetLen(const Node *head)
{ 
    Node *p = NULL;
    int count = 0;

    assert(NULL != head);

    p = head->pnext;
    while(NULL != p)
    {
        ++count;
        p = p->pnext;
    }

    return count;
}

int my_strcmp(const char *dest, const char *source)
{
    assert((NULL != dest)
            && (NULL != source));

    if(('\0' == (*dest)) && ('\0' == (*source)))
    {
        return EQUAL;
    }
    else if( (tolower(*dest) > tolower(*source)) )
    {
        return (('\0' != *dest) ? BIGER : SMALLER);
    }
    else if( (tolower(*dest) < tolower(*source)) )
    {
        return (('\0' != *source) ?  SMALLER : BIGER);
    }
    else
    {
        return my_strcmp((dest+1), (source+1));
    }
}

bool SortList(Node *head)
{
    Node *current = NULL;//point to the current node
    Node *prior = NULL;//point to the current node's prior node
    Node *next = NULL;//point to the current node's next node
    Node *renext = NULL;//point to the current node's next's next node
    int listLen = 0;
    int i = 0;
    int j = 0;

    assert(NULL != head);

    listLen = GetLen(head);

    for( ; i<listLen; ++i)//bubble sort
    {
        for(j = 0, prior = head, current = head->pnext;\
                j < (listLen - 1 - i); ++j)
        {
            if(my_strcmp(current->pdata, current->pnext->pdata) > 0)
            {
                next = current->pnext;
                renext = next->pnext;

                prior->pnext = next;
                next->pnext = current;
                current->pnext = renext;

                prior = next;
            }
            else
            {
                prior = current;
                current = current->pnext;
            }
        }
    }

    return true;
}

bool CleanList(Node *head)
{
    Node *p = NULL;

    assert(NULL != head);

    p = head->pnext;

    while(p != NULL)
    {
        head->pnext = p->pnext;
        free(p->pdata);
        free(p);
        p = head->pnext;
    }

    return true;
}

bool DestroyList(Node **phead)
{
    assert(NULL != *phead);

    if(CleanList(*phead))
    {
        free(*phead);
        *phead = NULL;
    }
    else
    {
        return false;
    }

    return true;
}
