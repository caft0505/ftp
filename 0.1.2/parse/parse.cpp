#include "parse.h"

u_int parse::parse_digit(const u_char * str,
                            int strlen,
                            int typesize)
{
    if( (NULL == str) || (typesize <= 0) )
    {
        cerr << "error arguments" << endl;
        exit(1);
    }

    int i = 0;
    int min = ( (strlen <= typesize) ?
                    strlen : typesize );
    u_int res = 0;

    if( is_small_endian() )
    {
        while(min-- > 0)
        {
            res *= 10;
            res += str[min];
        }
    }
    else
    {
        while( i < min )
        {
            res *= 10;
            res += str[i++];
        }
    }

    return res;
}

int parse::parse_str(char * str, int strlen,
                                    u_int value)
{
    if( (NULL == str) || (strlen <= 0) )
    {
        cerr << "error argument" << endl;
        return -1;
    }

    int i = 0;
    char * tmp = (char *)&value;
    int min = ( (strlen <= sizeof(u_int)) ?
                        strlen : sizeof(u_int) );

    if( is_small_endian() )
    {
        while( i < min )
        {
            str[i] = tmp[i];
            ++i;
        }
    }
    else
    {
        while( min-- > 0)
        {
            str[i++] = tmp[min];
        }
    }

    return 0;
}
