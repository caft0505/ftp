#include "myread.h"

//Show more file name
int ShowMore(Node *head)
{
    int i	            = 0;
    int j	            = 0;
    int k	            = 0;
    int row	            = 0;
    int col	            = 0;
    Style style         = {0};
    struct winsize size = {0};
    Node *pSlow         = NULL;
    Node *pFast         = NULL;

    assert(NULL != head);

    if(-1 == GetTermSize(&size))
    {
        return -1;
    }

    if(! SortList(head))
    {
        return -1;
    }

    style.row = size.ws_row;
    style.col = size.ws_col;

    if(-1 == GetShowStyle(head, &style))
    {
        return -1;
    }

    row = style.row;
    col = style.col;

    pSlow = head->pnext;

    puts("");

    //Print 'row' rows(k)
    //and 'col' cols(j)
    for(k = 0;
        (k < row) && (NULL != pSlow);
        ++k, pSlow = pSlow->pnext)
    {
        pFast = pSlow;

        //Every row has 'col' cols.
        for(j = 0; j < col; ++j)
        {
            if(NULL != pFast)
            {
                printf("%-*s", style.blank[j],
                                pFast->pdata);
            }

            //Get next data to print.
            for(i = 0;
                (i < row) && (NULL != pFast);
                ++i, pFast = pFast->pnext)
            {
                NULL;
            }
        }

        puts("");
    }

    return 0;
}
