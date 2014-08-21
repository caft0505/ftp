#ifndef _DIGIT_STRING_H_
#define _DIGIT_STRING_H_

#include "../afxstd.h"

// Parse class.
// The Parse is useful very much, it can
// get a digit from the string specified
// and can also get a result string that
// translate from the digit specified.
// Attention, it ignore the big endian
// and small endian, because it use itself
// rules.
// The rules is that the bit lower in
// digit at the byte lower in the string.
class parse
{
public:
    typedef unsigned char   u_char;
    typedef unsigned int    u_int;
    typedef unsigned short  u_short;

public:
    parse() {}
    ~parse() {}

private:
    // If the host machine is small endian type
    // it return 1, else return 0.
    static int is_small_endian()
    {
        short tmp = 0x1122;
        return ( 0x22 == *(char *)&tmp );
    }

    // Return the digit gets from the str. 
    static u_int parse_digit( const u_char * str,
                                int strlen,
                                int typesize );

public:
    // Return value, which type is int, gets
    // from the str. 
    static u_int parse_int( const u_char * str,
                                    int strlen )
    {
        return parse_digit( str, strlen, sizeof(int) );
    }

    // Return value, which type is short, gets
    // from the str. 
    static u_short parse_short( const u_char * str,
                                    int strlen )
    {
        return parse_digit( str, strlen, sizeof(short) );
    }

    // Fill the str refer to value,
    // If ok it return 0, else return -1.
    static int parse_str( char * str,
                            int strlen,
                            u_int value ); 
};

#endif
