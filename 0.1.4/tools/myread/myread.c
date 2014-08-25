#include "myread.h"

int InitKeyboard(struct termios *termios_p)
{
    struct termios ntermios_p;

    if(-1 == tcgetattr(0,termios_p))
    {
        perror("ERROR(tcgetattr)");
        return -1;
    }

    ntermios_p = *termios_p;
    ntermios_p.c_lflag &= ~ECHO;
    ntermios_p.c_lflag &= ~ICANON;
    ntermios_p.c_cc[VMIN] = 1;
    ntermios_p.c_cc[VTIME] = 0;

    if(-1 == tcsetattr(0, TCSANOW, &ntermios_p))
    {
        perror("ERROR(tcsetattr)");
        return -1;
    }

    return 0;
}

int CloseKeyboard(struct termios termios_p)
{
    if(-1 == tcsetattr(0, TCSANOW, &termios_p))
    {
        perror("ERROR(tcgetattr)");
        return -1;
    }

    return 0;
}

int my_read(Node *cmdHead, const char *info,
        char *cmd, int cmdMax)
{
    char och	  	   = 0;
    char nch	  	   = 0;
    int err		   = 0;
    int show		   = 0;
    int cursorPos	   = 0;
    int cmdPos	  	   = 0;
    int up		   = 0;
    int direction	   = 0;
    char lastBuf[BUFFSIZE] = {0};
    char path[BUFFSIZE]	   = {0};
    char *tail		   = NULL;
    Node *resultHead	   = NULL;

    struct termios termios_p;

    assert(NULL != info);
    assert(NULL != cmd);

    if(-1 == InitKeyboard(&termios_p))
    {
        return -1;
    }

    if(! InitList(&resultHead) )
    {
        err = -1;
        goto END;
    }

    //Print mark information.
    printf("%s", info);
    fflush(stdout);

    while((cmdPos < cmdMax) &&
            ('\n' != (nch = getchar())))
    {
        //Ignore 'Ctrl+' alpha.
        if(((nch >= 1) && (nch <= 2))\
                || (nch >= 4) && (nch <= 8)
                || ((nch >= 10) && (nch <= 26)))
        {
            och = nch;
            direction = 0;
            continue;
        }

        //Button 'Tab' is '\t'
        if('\t' != nch)
        {
            och = nch;

            switch(nch)
            {
                //This is button 'Backspace'
                case 127 :
                    {
                        direction = 0;

                        if((cmdPos > 0)
                                && (cursorPos > 0 ))
                        {
                            Delete(cmd, cursorPos);
                            --cursorPos;
                            --cmdPos;
                        }
                    }

                    break;

                    //This is button 'up'/'down'
                    //             'left'/'right':
                    //   'up'-->'^[[A', 'down'-->'^[[B'
                    //'right'-->'^[[C', 'left'-->'^[[D'
                    /*            '^[' = 27          */
                case 27 :// --> '^['
                    direction |= 1;//01
                    break;

                case '[' :
                    {
                        if( ! (direction & 1) )
                        {
                            show = 1;
                        }
                        else
                        {
                            direction |= 2;//10
                        }
                    }

                    break;

                case 'A' :
                    {
                        if( ! (direction & 3) )
                        {
                            show = 1;
                        }
                        else
                        {
                            if(up < GetLen(cmdHead))
                            {
                                if(0 == up)
                                {
                                    strcpy(lastBuf, cmd);
                                }

                                ++up;

                                if(1 == UpOrDown(cmdHead, cursorPos,
                                            up, lastBuf, cmd))
                                {
                                    cmdPos = strlen(cmd);
                                    cursorPos = cmdPos;
                                }
                            }
                        }
                    }

                    break;

                case 'B' :
                    {
                        if( ! (direction & 3) ) 
                        {
                            show = 1;
                        }
                        else
                        {
                            if(up > 0)
                            {
                                --up;

                                if(1 == UpOrDown(cmdHead, cursorPos,
                                            up, lastBuf, cmd)) 
                                {
                                    cmdPos = strlen(cmd);
                                    cursorPos = cmdPos;
                                }
                            }
                        }
                    }

                    break;

                case 'C' :
                    {
                        if( ! (direction & 3) )
                        {
                            show = 1;
                        }
                        else
                        {
                            if(cursorPos < cmdPos)
                            {
                                ++cursorPos;
                                putchar(27);
                                putchar('[');
                                putchar(nch);
                                fflush(stdout);
                            }
                        }
                    }

                    break;

                case 'D' :
                    {
                        if( ! (direction & 3) )
                        {
                            show = 1;
                        }
                        else
                        {
                            if(cursorPos > 0)
                            {
                                --cursorPos;
                                printf("\b");
                                fflush(stdout);
                            }
                        }
                    }

                    break;

                default :
                    show = 1;
                    break;
            }

            if(1 == show)
            {
                show = 0;

                direction = 0;

                Add(cmd, cursorPos, nch);
                ++cursorPos;
                ++cmdPos;
            }
        }
        else
        {
            strcpy(path, GetTail(cmd, ' '));

            //If no blank-space appear in
            //cmd before clicking button
            //'Tab', do nothing.
            if( strcmp(cmd, path)
                    && (cmdPos == cursorPos) )
            {
                if( -1 ==  Tab( resultHead,\
                            info,
                            path,\
                            och == nch,\
                            cmd ) )
                {
                    ;
                }

                cmdPos = strlen(cmd);
                cursorPos = cmdPos;
            }

            direction = 0;
            och = nch;
        }

        if(! CleanList(resultHead))
        {
            err = -1;
            goto END;
        }
    }

    if(0 == cmdPos)
    {
        cmd[cmdPos] = '\n';
    }

    if(cmdPos >= cmdMax)
    {
        cmd[cmdMax-1] = '\0';

        err = -1;
    }

    printf("\n");

END:
    DestroyList(&resultHead);

    if(0 != strcmp(cmd, "\n"))
    {
        tail = GetVal(cmdHead, GetLen(cmdHead));

        if(NULL != tail)
        {
            if((0 != strcmp(cmd, tail))
                    && (! AddNode(cmdHead, cmd)))
            {
                err = -1;
            }
        }
        else
        {
            if(! AddNode(cmdHead, cmd))
            {
                err = -1;
            }
        }
    }

    return (CloseKeyboard(termios_p) ? -1 : err);
}
