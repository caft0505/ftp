#include "formart.h"

//Get current terminal's size
int GetTermSize(struct winsize *size)
{
    assert(NULL != size);

    if(-1 == ioctl(STDOUT_FILENO, TIOCGWINSZ, size))
    {
        perror("ERROR(ioctl)");
        return -1;
    }

    return 0;
}

//Get Len from 'cur' to 'cur+num'
int GetNodeLen(Node *cur, int num, Len *len)
{
    int i       = 0;
    int sum     = 0;
    int max     = 0;
    int datalen = 0;
    Node *p     = NULL;

    assert(NULL != len);

    if(NULL == cur)
    {
        return 0;
    }

    p = cur;

    for(i = 0;
        (i < num) && (NULL != p);
         ++i, p = p->pnext)
    {
        if(max < (datalen = strlen(p->pdata)))
        {
            max = datalen;
        }

        sum += datalen;
    }

    len->sumlen = sum;
    len->maxlen = max;

    return 0;
}

//Get show-style
int GetShowStyle(const Node *head, Style *style)
{
    int i       = 0;
    int j       = 0;
    int k       = 0;
    int t       = 0;
    int termRow = 0;
    int termCol = 0;
    int nRow    = 0;
    int nCol    = 0;
    int sumlen  = 0;
    int listLen = 0;
    Len len     = {0};
    Node *pSlow = NULL;
    Node *pFast = NULL;

    assert(NULL != head);
    assert((NULL != style)
        && (style->row > 0)
        && (style->col > 0));

    termRow = style->row;//Current terminal's row
    termCol = style->col;//Current terminal's col
    listLen = GetLen(head);

    if(-1 == GetNodeLen(head->pnext, listLen, &len))
    {
        return -1;
    }

    //One line can hold all node
    if((len.sumlen + (listLen - 1) * 2) <= termCol)
    {
        style->row = 1;
        style->col = listLen;

        i     = 0;
        pFast = head->pnext;
        while(NULL != pFast)
        {
            style->blank[i++] = strlen(pFast->pdata)+2;
            pFast = pFast->pnext;
        }

        return 0;
    }
    else
    {
        nRow = 1;

        while(1)
        {
            pSlow  = head->pnext;

            //Test a new row and new col
            nRow += 1;

            nCol = listLen/nRow + (listLen%nRow ? 1 : 0);

            pFast = pSlow;

            //Scan every col
            for(i = 0;
                (i < nCol)
                 && (NULL != pFast);
                  ++i)
            {
                //Get the maxlen of node in this col
                if(-1 == GetNodeLen(pFast, nRow, &len))
                {
                    return -1;
                }

                //Count the max length of the line
                sumlen += len.maxlen;
                //Add tow blank-space
                sumlen += 2;
                //Get 'len.maxlen+2' to print
                style->blank[t++] = len.maxlen + 2;

                //If 'nRow' lines can't hold all node
                //then restart to test another new row
                //(is 'nRow+1') and new col
                if(sumlen > termCol)
                {
                    sumlen = 0;
                    t = 0;
                    memset(style->blank, 0,
                           sizeof(style->blank));

                    break;
                }

                //Go to next col.
                //But if the col is the last col, don't
                //skip to next node.
                for(j = 0;
                    (i < (nCol-1))//This is the last col
                    && (j < nRow)
                    && (NULL != pFast);
                    ++j, pFast = pFast->pnext)
                {
                    NULL;
                }
            }

            //Have already scanned all of the cols
            //and all node can be holded in the 'nRow'
            //lines.
            if(i == nCol)
            {
                break;
            }
        }

        //Get correct row and col to ouput.
        style->row = nRow;
        style->col = nCol;
    }

    return 0;
}
