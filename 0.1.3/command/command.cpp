#include "command.h"

command_pair::command_pair(const char * cmd_name,
                                    char cmd_type)
{
    if(NULL != cmd_name)
    {
        command_name = cmd_name;
        command_type = cmd_type;
    }
}

command_pair::command_pair(const command_pair & new_cmd)
{
    command_name = new_cmd.command_type;
    command_type = new_cmd.command_type;
}

command_pair & command_pair::operator=(
                    const command_pair & cmd)
{
    if(this != &cmd)
    {
        this->command_name = cmd.command_name;
        this->command_type = cmd.command_type;
    }

    return *this;
}

command::~command()
{
    int num = get_command_num();
    for(int i = 0; i < num; ++i)
    {
        delete m_command[i];
        //m_command[i] = NULL;
    }
}

int command::search_command(const char * cmd)
{
    int index = 0;

    while( ( index < get_command_num() )
         && (*m_command[index] != cmd) )
    {
        ++index;
    }

    //cout << index << endl;

    return ( (index < get_command_num()) ? index : -1 );
}

void command::register_command(const char * cmd_name,
                                    char cmd_type)
{
    command_pair * new_pcmd = new command_pair(cmd_name, cmd_type);
    if(NULL == new_pcmd)
    {
        cerr << "bad alloc in "
            << __FILE__ << " : "
            << __LINE__ << endl;

        return ;
    }

    m_command.push_back(new_pcmd); 
}

char command::get_command_type(const char * cmd)
{
    int index = search_command(cmd);
    if(-1 == index)
    {
        cout << "no such command" << endl;
        return CMD_ERR;
    }

    return m_command[index]->get_command_type();
}
