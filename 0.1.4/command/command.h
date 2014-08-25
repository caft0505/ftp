#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "../afxstd.h"
#include "../protocol.h"

class command_pair
{
public:
    command_pair() {}

    command_pair(const char * cmd_name, char cmd_type);

    command_pair(const command_pair & new_cmd);

    ~command_pair() {}

public:
    command_pair & operator=(const command_pair & cmd);

    int operator==(const command_pair & cmd)
    {
        return ( (command_name == cmd.command_name)
            && (command_type == cmd.command_type) );
    }
    int operator==(const command_pair & cmd) const
    {
        return ( (command_name == cmd.command_name)
            && (command_type == cmd.command_type) );
    }

    int operator!=(const command_pair & cmd)
    {
        return ( !(*this == cmd) );
    }
    int operator!=(const command_pair & cmd) const
    {
        return ( !(*this == cmd) );
    }

    int operator==(const char * cmd)
    {
        return (command_name == cmd);
    }
    int operator==(const char * cmd) const
    {
        return (command_name == cmd);
    }

    int operator!=(const char * cmd)
    {
        return (command_name != cmd);
    }
    int operator!=(const char * cmd) const
    {
        return (command_name != cmd);
    }

public:
    char get_command_type() const
    {
        return command_type;
    }

    const string & get_command_name() const
    {
        return command_name;
    }

private:
    string      command_name;
    char        command_type;
};

class command
{
public:
    command() {}
    ~command();

private:
    int get_command_num()
    {
        return m_command.size();
    }

    int  search_command(const char * cmd);

public:
    void register_command(const char * cmd_name, char cmd_type);
    char get_command_type(const char * cmd);

private:
    deque<command_pair *>   m_command;
};

#endif
