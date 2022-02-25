#include "include/proclib.hpp"
#include "include/filedescriptor.hpp"

#include <unordered_map>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

/**
 * @brief analyze memory
 *
 * @param __buffer
 * @param __find
 * @param __offset
 * @param __type
 * @return int
 */
int RemoteProcess::Analyse(char *__buffer, std::string __find, off_t __offset, uint8_t __type,
                           uint64_t lenght, std::vector<off_t> &offset)
{
    switch (__type)
    {
    case sizeof(char):
        offset.clear();
        if (!isdigit(__find[0]) && __find.size() == 1)
        {
            for (uint64_t i = 0; i < lenght; i++)
            {
                if (__buffer[i] == __find[0])
                    offset.push_back(__offset);
                __offset++;
            }
        }
        else
            throw std::runtime_error("Error, caracter '" + __find + "' not valid");
        break;
    case sizeof(int):
        offset.clear();
        {
            try
            {
                int find = std::stoi(__find);
                if (find < INT_MAX && find > 0)
                {
                    for (uint64_t i = 0; i < lenght; i++)
                    {
                        if ((int)__buffer[i] == find)
                            offset.push_back(__offset);
                        __offset++;
                    }
                }
            }
            catch (std::exception &error)
            {
                throw std::runtime_error("Error, caracter '" + __find + "' not valid");
            }
        }
        break;
    case sizeof(uint16_t):
        offset.clear();
        {
            try
            {
                int16_t find = (uint16_t)std::stoi(__find);
                if (find <= UINT16_MAX && find > 0)
                {
                    for (uint64_t i = 0; i < lenght; i++)
                    {
                        if ((uint16_t)__buffer[i] == find)
                            offset.push_back(__offset);
                        __offset++;
                    }
                }
            }
            catch (std::exception &error)
            {
                throw std::runtime_error("Error, caracter '" + __find + "' not valid");
            }
        }
        break;
    case sizeof(uint64_t):
        offset.clear();
        {
            try
            {
                int64_t find = (uint64_t)std::stoi(__find);
                if (find < UINT64_MAX && find > 0)
                {
                    for (uint64_t i = 0; i < lenght; i++)
                    {
                        if ((uint64_t)__buffer[i] == find)
                            offset.push_back(__offset);
                        __offset++;
                    }
                }
            }
            catch (std::exception &error)
            {
                throw std::runtime_error("Error, caracter '" + __find + "' not valid");
            }
        }
        break;
    case sizeof(std::string):
        offset.clear();
        {
            std::string str;
            for (uint64_t i = 0; i < lenght; i++)
            {
                if ((char)__buffer[i] == (char)__find[0])
                {
                    for (std::size_t i = 0; i < __find.size(); i++)
                        str += __buffer[i += i];
                    if (strncmp(str.data(), __find.data(), __find.size()))
                        offset.push_back(__offset);
                }
                __offset++;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

RemoteProcess::RemoteProcess()
{
    struct stat st;
    m_hasProcMem = stat("/proc/self/mem", &st) != 0;
}

/**
 * @brief Attach to a remote process
 * @param pid_t __pid to be attached inside the system
 * @return int
 *
 * if the current process is unable to attach to the target pid, it returns OPEN_FAIL
 * if the remote process were attached successfully but it does not received the SIGSTOP signal
 * it will also returns OPEN_FAIL, otherwise it will return OPEN_SUCCESS
 */
int RemoteProcess::openProcess(pid_t __pid)
{
    m_proc.pid = __pid;
    m_status = OPEN_FAIL;

    long requests = ptrace(PTRACE_ATTACH, m_proc.pid, NULL, NULL);
    if (requests < 0)
        throw std::runtime_error("Not possible attach pid " + std::to_string(m_proc.pid));

    // wait to the attach ends
    int wstatus;
    waitpid(m_proc.pid, &wstatus, 0);

    if (WIFSTOPPED(wstatus))
    {
        // process has been attached and stopped, continue normally
        ptrace(PTRACE_CONT, m_proc.pid, NULL, NULL);
        m_status = OPEN_SUCCESS;
    }

    return m_status;
}

/**
 * @brief Attach to a remote process
 * @param pid_t __pid to be attached inside the system
 * @return int
 *
 * if the current process is unable to attach to the target pid, it returns OPEN_FAIL
 * if the remote process were attached successfully but it does not received the SIGSTOP signal
 * it will also returns OPEN_FAIL, otherwise it will return OPEN_SUCCESS
 */
RemoteProcess::~RemoteProcess()
{
    if (m_status == OPEN_SUCCESS)
        ptrace(PTRACE_DETACH, m_proc.pid, NULL, NULL);
}

/**
 * @brief Read the memory of the remote process
 * @param start offset to be read into the remote process
 * @param Data* data class to save the memory data
 * @return int
 *
 * If the remote memory cannot be read, it will returns READ_FAIL otherwise it will returns READ_SUCESS. The error cause can be found at the
 * errno variable, which is set by ptrace.
 */
int RemoteProcess::readMem(off_t start, Data *data)
{
    if (m_status == OPEN_FAIL)
        return READ_FAIL;

    long b;
    for (uint i = 0; i < data->m_size; ++i)
    {
        b = ptrace(PTRACE_PEEKDATA, m_proc.pid, start + i, NULL);
        if (b == -1)
            return READ_FAIL;

        data->write((uint8_t)b);
    }

    return READ_SUCCESS;
}

/**
 * @brief Write into a remote process memory
 *
 * @param start offset to be written into
 * @param data class with the target data to be write
 * @return int
 *
 * If the remote memory cannot be written, it will returns WRITE_FAIL, otherwise WRITE_SUCESS. The error cause can be found at the
 * errno variable, which is set by ptrace.
 */
int RemoteProcess::writeMem(off_t start, Data *data)
{
    if (m_status != OPEN_SUCCESS)
        return WRITE_FAIL;

    long s;
    for (uint i = 0; i < data->m_size; ++i)
    {
        s = ptrace(PTRACE_POKEDATA, m_proc.pid, start + i, data->read());
        if (s < 0)
            return WRITE_FAIL;
    }

    return WRITE_SUCCESS;
}

/**
 * @brief find value in memory if not found return NOT_FOUND, else return FOUND
 *
 * @param start offset to start looking
 * @param data if found, it will be stored in data
 * @return int
 */
int RemoteProcess::findMem(off_t start, uint64_t length, uint8_t type, std::string find, std::vector<off_t> &offsets)
{
    if (m_status != OPEN_SUCCESS)
        return NOT_FOUND;

    if (!m_hasProcMem)
    {
        std::string name = PROC + std::to_string(m_proc.pid) + "/mem";
        int fd = open(name.data(), O_RDONLY);
        if (fd > 0)
        {
            char *buffer;
            try
            {
                buffer = new char[length];
                memset(buffer, 0, length); // clear memory for allocation
                ssize_t read = pread64(fd, buffer, length, start);
                if (read < 0)
                    return READ_FAIL;
                else
                    Analyse(buffer, find, start, type, length, offsets);
            }
            catch (std::exception &error)
            {
                throw std::runtime_error(error.what());
                delete[] buffer;
                return READ_FAIL;
            }

            close(fd);
            delete[] buffer;
        }
        else
        {
            throw std::runtime_error("Error not open file " + name);
            return OPEN_FAIL;
        }
    }
    else
    {
        throw std::runtime_error("Not supported find memory, directory 'proc/" + std::to_string(m_proc.pid) + "/mem' not found, \n maProc not support for read mem with ptrace");
        return READ_FAIL;
    }

    return READ_SUCCESS;
}

/**
 * @brief close pid, if pid valid stop attacking
 * 
 */
void RemoteProcess::closePid()
{
    if (m_status == OPEN_SUCCESS && m_proc.pid != 0)
        ptrace(PTRACE_DETACH, m_proc.pid, NULL, NULL);
}

Data::Data(uint __size)
{
    m_size = __size;
    m_buff = new uint8_t[m_size];
    m_curr = 0;
}

Data::~Data()
{
    delete[] m_buff;
}

void Data::write(uint8_t b)
{
    if (m_curr++ > m_size) // not enough sspace
        return;

    *(m_buff++) = b;
}

uint8_t Data::read()
{
    m_curr = m_curr % m_size; // cyclic reading
    return m_buff[m_curr++];
}

void Data::clear()
{
    memset(m_buff, 0, m_size);
}