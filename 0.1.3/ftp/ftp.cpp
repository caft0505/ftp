#include "ftp.h"

// Return the num of the reply bytes.
int ftp::reply(char pack_type)
{
    int res = 0;
    char buff[2] = {0};

    buff[0] = buff[1] = pack_type;

    res = send(m_fd, buff, 2, 0);
    if(2 != res)
    {
        cerr << "error send in "
            << __FILE__ << " : "
            << __LINE__ << endl;
    }

    return res;
}

char ftp::acceptable()
{
    int res = 0;
    char buff[2] = {0};

    res = recv(m_fd, buff, 2, 0);
    if(2 != res)
    {
        cerr << "error recv in "
            << __FILE__ << " : "
            << __LINE__ << endl;
    }

    if(buff[0] != buff[1])
    {
        ;// Error package.
    }

    return buff[0];
}

int ftp::acceptable(char pack_type)
{
    return is_pack_type(pack_type, acceptable() );
}

// Return received size.
int ftp::recv_data()
{
    char buff[BUFFSIZE] = {0};

    int res = 0;

    m_buff.clear();

    while( 1 )
    {
        bzero(buff, BUFFSIZE);

        res = recv(m_fd, buff, BUFFSIZE, 0);
        if(res < 0)
        {
            cerr << "error recv in "
                << __FILE__ << " : "
                << __LINE__ << endl;

            m_buff.clear();
            m_analyser.clear();

            return -1;
        }
        else if(0 == res)
        {
            break;
        }

        add_buff(buff, res);

        m_analyser.load_data(m_buff);

        if(m_buff.size() == m_analyser.get_pack_size())
        {
            if(m_analyser.checkout())
            {
                reply(PT_OK);

                // Delete the checkcode.
                m_buff.pop_back();

                break;
            }

            m_buff.clear();
            m_analyser.clear();

            reply(PT_ERROR);
        }
    }

    return m_buff.size();
}

// Return sended size.
int ftp::send_data()
{
    char buff[BUFFSIZE] = {0};
    int size = get_buff(buff, BUFFSIZE);
    int send_size = 0;
    int res = 0;

    while( 1 )
    {
        while(send_size < size)
        {
            res = send(m_fd, buff + send_size, size - send_size, 0);
            if(res < 0)
            {
                cerr << "error recv in "
                    << __FILE__ << " : "
                    << __LINE__ << endl;

                return -1;
            }
            else if(0 == res)
            {
                return -1;
            }

            send_size += res;
        }

        // Send data is correct.
        if( acceptable() )
        {
            break;
        }

        // Send data is incorrect.
        send_size = 0;
    }

    return send_size;
}

// Return buff size.
int ftp::get_buff(char * buff, int blen)
{
    if(NULL == buff || blen <= 0)
    {
        cerr << "error arguments in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        return -1;
    }

    int i = 0;

    while(i < blen && i < m_buff.size())
    {
        buff[i] = m_buff[i];

        ++i;
    }

    return i;
}

int ftp::add_buff(const char * buff, int blen)
{
    if(NULL == buff || blen <= 0)
    {
        cerr << "error arguments in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        return -1;
    }

    int i = 0;
    for( ; i < blen; ++i)
    {
        m_buff.push_back(buff[i]);
    }

    return blen;
}

int ftp::set_buff(const char * buff, int blen)
{
    m_buff.clear();

    return add_buff(buff, blen);
}

int ftp::recv_file_info(file_info & f_info)
{
    int res = 0;
    char buff[BUFFSIZE] = {0};

    if( (res = recv(m_fd, buff, BUFFSIZE, 0)) <= 0)
    {
        return -1;
    }

    set_buff(buff, res);
    m_analyser.load_data(buff, res);

    m_analyser.get_file_name(f_info.file_name, BUFFSIZE);
    f_info.file_size = m_analyser.get_file_size();
    f_info.file_mode = m_analyser.get_file_mode();

    return 0;
}

int ftp::recv_file()
{
    file_info f_info = {0};

    m_analyser.get_file_name(f_info.file_name, BUFFSIZE);
    f_info.file_size = m_analyser.get_file_size();
    f_info.file_mode = m_analyser.get_file_mode();

    int wfd = open(f_info.file_name,
                O_WRONLY | O_CREAT | O_LARGEFILE,
                                f_info.file_mode);
    if(wfd < 0)
    {
        cerr << "error open in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        return -1;
    }

    int recv_size = 0;
    int pipefd[2];

    if(-1 == pipe(pipefd))
    {
        cerr << "error pipe in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        close(wfd);
        return -1;
    }

    int res = 0;

    if(recv_size < f_info.file_size)
    {
        while(recv_size < f_info.file_size)
        {
            res = splice(m_fd, NULL, pipefd[1], NULL,
                        f_info.file_size - recv_size, 
                SPLICE_F_MORE | SPLICE_F_MOVE | SPLICE_F_NONBLOCK);

            if(-1 == res)
            {
                cerr << "error splice in "
                    << __FILE__ << " : "
                    << __LINE__ << endl;
                close(wfd);
                return -1;
            }
            else if(0 == res)
            {
                continue;
            }

            res = splice(pipefd[0], NULL, wfd, NULL, res,
                SPLICE_F_MORE | SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
            if(-1 == res)
            {
                cerr << "error splice in "
                    << __FILE__ << " : "
                    << __LINE__ << endl;
                close(wfd);
                return -1;
            }
            else if(0 == res)
            {
                ;// Warning , ignore.
            }

            recv_size += res;

            // Print the receive rate.
            printf("\033\r\033[K");
            cout << "receive file: "
                << fixed << setprecision(0)
                << (100.0 * recv_size / f_info.file_size)
                << " %";
            cout.flush();
        }

        cout.unsetf(ostream::floatfield);

        cout << endl;
    }
    else// For empty file.
    {
        cout << "receive file: " << "100 %" << endl;
    }

    close(pipefd[0]);
    close(pipefd[1]);
    close(wfd);

    return 0;
}

int ftp::send_file_info(const char * file_name)
{
    // Get file's informatioin.
    struct stat file_buf = {0};
    if(0 != stat(file_name, &file_buf) )
    {
        cerr << "error stat in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        return -1;
    }

    const char *p = NULL;
    if(NULL != (p = strrchr(file_name, '/')) )
    {
        file_name = p + 1;
    }

    m_buff.clear();

    // Add the package type.
    m_buff.push_back(PT_UPREQ);

    // Add the length of the file name.
    m_buff.push_back(strlen(file_name));

    // Add the file's name.
    m_buff.insert(m_buff.end(), file_name,
                    file_name + strlen(file_name));

    char buff[BUFFSIZE] = {0};

    // Add the file size.
    m_analyser.parse_str(buff, sizeof(int),
                                file_buf.st_size);
    m_buff.insert(m_buff.end(), buff,
                                buff + sizeof(int));

    bzero(buff, BUFFSIZE);

    // Add the file mode.
    m_analyser.parse_str(buff, sizeof(int),
                                file_buf.st_mode);
    m_buff.insert(m_buff.end(), buff,
                                buff + sizeof(int));

    // Add the checkcode.
    m_analyser.load_data(m_buff);
    m_buff.push_back(m_analyser.check());

    if( send_data()<= 0 )
    {
        return -1;// Send package error.
    }

    return 0;
}

int ftp::send_file(const char * file_name)
{
    if(NULL == file_name)
    {
        cerr << "error arguments in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        return -1;
    }

    // Get file's informatioin.
    struct stat file_buf = {0};
    if(0 != stat(file_name, &file_buf) )
    {
        cerr << "error stat in "
            << __FILE__ << " : "
            << __LINE__ << endl;
        return -1;
    }

    int file_size = file_buf.st_size;

    int rfd = open(file_name, O_RDONLY | O_LARGEFILE);

    int remain_size = file_size;
    int res = 0;

    if(remain_size > 0)
    {
        while( remain_size > 0 )
        {
            res = sendfile(m_fd, rfd, NULL,
                    ( (remain_size < SENDSIZE ?
                        remain_size : SENDSIZE) ) );
            if(res < 0)
            {
                cerr << "error sendfile in " << __FILE__ << " : "
                    << __LINE__ << endl;
                close(rfd);
                return -1;
            }

            remain_size -= res;

            // Print the send rate.
            printf("\033\r\033[K");
            cout << "send file: "
                << fixed << setprecision(0)
                //<< (1.0 * send_size / file_size * 100)
                << (1 - 1.0 * remain_size / file_size) * 100
                << " %";
            cout.flush();
        }

        cout << endl;
            
        cout.unsetf(ostream::floatfield);
    }
    else // For empty file.
    {
        cout << "send file: " << "100 %" << endl;
    }

    close(rfd);

    return 0;
}
