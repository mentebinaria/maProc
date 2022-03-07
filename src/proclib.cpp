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
 * @brief routine to analyze memory,
 * with the appropriate types char, int, int16, int64,'string'I
 *
 * @param buffer memory for analysis
 * @param find what to look for
 * @param offset where to start for counting
 * @param type type char, int64 ...
 * @return int
 */
void RemoteProcess::Analyse(char *buffer, std::string find, off_t offset, uint8_t type,
                            uint64_t lenght, std::vector<off_t> &save)
{
    switch (type)
    {
    case sizeof(char):
        save.clear();
        if (!isdigit(find[0]) && find.size() == 1)
        {
            for (uint64_t i = 0; i < lenght; i++)
            {
                if (buffer[i] == find[0])
                    save.push_back(offset);
                offset++;
            }
        }
        else
            throw std::runtime_error("Error, caracter '" + find + "' not valid");
        break;
    case sizeof(int):
        save.clear();
        {
            try
            {
                int findP = std::stoi(find);
                if (findP < INT_MAX && findP > 0)
                {
                    for (uint64_t i = 0; i < lenght; i++)
                    {
                        if ((int)buffer[i] == findP)
                            save.push_back(offset);
                        offset++;
                    }
                }
            }
            catch (std::exception &error)
            {
                throw std::runtime_error("Error, caracter '" + find + "' not valid");
            }
        }
        break;
    case sizeof(uint16_t):
        save.clear();
        {
            try
            {
                int16_t findP = (uint16_t)std::stoi(find);
                if (findP <= UINT16_MAX && findP > 0)
                {
                    for (uint64_t i = 0; i < lenght; i++)
                    {
                        if ((uint16_t)buffer[i] == findP)
                            save.push_back(offset);
                        offset++;
                    }
                }
            }
            catch (std::exception &error)
            {
                throw std::runtime_error("Error, caracter '" + find + "' not valid");
            }
        }
        break;
    case sizeof(uint64_t):
        save.clear();
        {
            try
            {
                int64_t findP = (uint64_t)std::stoi(find);
                if (findP < UINT64_MAX && findP > 0)
                {
                    for (uint64_t i = 0; i < lenght; i++)
                    {
                        if ((int64_t)buffer[i] == findP)
                            save.push_back(offset);
                        offset++;
                    }
                }
            }
            catch (std::exception &error)
            {
                throw std::runtime_error("Error, caracter '" + find + "' not valid");
            }
        }
        break;
    case sizeof(std::string):
        save.clear();
        {
            std::string str;
            for (uint64_t it = 0; it < lenght; it++)
            {
                if (buffer[it] == find[0])
                {
                    for (std::size_t i = 0; i < find.size(); i++)
                        str += buffer[it + i];

                    if (str == find)
                        save.push_back(offset);
                }
                offset++;
            }
        }
        break;
    default:
        break;
    }
}

RemoteProcess::RemoteProcess()
{
    m_status = 0;
}

/**
 * @brief Attach to a remote process
 * @param pid_t __pid to be attached inside the system
 * @return int
 *
 */
int RemoteProcess::openProcess(pid_t __pid)
{
    m_proc.pid = __pid;
    m_status = OPEN_SUCCESS;
    struct stat st;
    m_proc.dirmem = PROC + std::to_string(m_proc.pid) + "/mem";

    if (stat("/proc/self/mem", &st) == 0)
    {
        m_proc.fd = open(m_proc.dirmem.data(), O_RDWR);
        if (m_proc.fd == -1)
            m_status = OPEN_FAIL;
    }
    else
        m_status = OPEN_FAIL;

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
    close(m_proc.fd);
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
int RemoteProcess::readMem(off_t start, off_t stop, Data *data)
{
    if (m_status == OPEN_FAIL)
        return READ_FAIL;

    size_t bsize = stop - start;
    pread(m_proc.fd, data->m_buff, bsize, start);

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
    if (m_status == OPEN_FAIL)
        return WRITE_FAIL;

    ssize_t write = pwrite(m_proc.fd, reinterpret_cast<const void *>(data->m_buff), data->m_size, start);
    if (write == -1)
        return WRITE_FAIL;

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
    if (m_status == OPEN_FAIL)
    {
        throw std::runtime_error("Error not open file " + m_proc.dirmem);
        return OPEN_FAIL;
    }
    else if (!m_hasProcMem)
    {
        char *buffer;
        try
        {
            buffer = new char[length];
            memset(buffer, 0, length); // clear memory for use memory
            if (pread(m_proc.fd, buffer, length, start) == -1)
                return READ_FAIL;
            else
                Analyse(buffer, find, start, type, length, offsets);
        }
        catch (std::exception &error)
        {
            delete[] buffer;
            throw std::runtime_error(error.what());
            return READ_FAIL;
        }

        delete[] buffer;
    }
    else
    {
        throw std::runtime_error("Not supported find memory, directory 'proc/" + std::to_string(m_proc.pid) + "/mem' not found, \n maProc not support for read mem with ptrace");
        return READ_FAIL;
    }

    return READ_SUCCESS;
}

/**
 * @brief stop pid
 *
 */
void RemoteProcess::stopPid(bool __enable)
{
    if (__enable == true && m_proc.pid != 0)
        kill(m_proc.pid, SIGSTOP);
    else
        kill(m_proc.pid, SIGCONT);
}

/**
 * @brief kill process
 *
 */
void RemoteProcess::killPid()
{
    kill(m_proc.pid, SIGKILL);
}

//
// part of the code in which we are going
// to move the buffer for writing to memory and reading
//

Data::Data(uint __size)
{
    m_size = __size;
    m_buff = new uint8_t[m_size];
    externalRef = false;
    m_curr = 0;
}

Data::Data(uint8_t *buff, uint size)
{
    externalRef = true;
    m_buff = buff;
    m_size = size;
    m_curr = 0;
}

Data::~Data()
{
    if (!externalRef)
        delete[] m_buff;
}

void Data::write(uint8_t b)
{
    if (m_curr++ == m_size)
        return;

    m_buff[m_curr] = b;
}

uint8_t *Data::read()
{
    return (uint8_t *)m_buff;
}

void Data::clear()
{
    memset(m_buff, 0, m_size);
}