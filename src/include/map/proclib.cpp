#include "proclib.hpp"
#include "src/include/fd/filedescriptor.hpp"
#include "src/include/memory/arena.hpp"

#include <unordered_map>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

Arena fast(ARENA_BUFFER);

/**
 * @brief routine to analyze memory,
 * with the appropriate p_types char, int, int16, int64,'string'I
 *
 * @param p_buffer memory for analysis
 * @param p_find what to look for
 * @param p_offsets where to p_start for counting
 * @param p_type p_type char, int64 ...
 * @return int
 */
void RemoteProcess::analyseMemory(char *p_buffer, std::string p_find, off_t p_offsets, uint8_t p_type,
                            uint64_t p_lenght, std::vector<off_t> &p_save)
{
    switch (p_type)
    {
    case sizeof(char):
        p_save.clear();
        for (uint64_t i = 0; i < p_lenght; i++)
        {
            if (p_buffer[i] == p_find[0])
                p_save.push_back(p_offsets);
            p_offsets++;
        }
        break;
    case sizeof(int):
        p_save.clear();
        {
            int p_findP = std::stoi(p_find);
            if (p_findP < INT_MAX && p_findP > 0)
            {
                for (uint64_t i = 0; i < p_lenght; i++)
                {
                    if ((int)p_buffer[i] == p_findP)
                        p_save.push_back(p_offsets);
                    p_offsets++;
                }
            }
        }
        break;
    case sizeof(uint16_t):
        p_save.clear();
        {
            int16_t p_findP = (uint16_t)std::stoi(p_find);
            if (p_findP <= UINT16_MAX && p_findP > 0)
            {
                for (uint64_t i = 0; i < p_lenght; i++)
                {
                    if ((uint16_t)p_buffer[i] == p_findP)
                        p_save.push_back(p_offsets);
                    p_offsets++;
                }
            }
        }
        break;
    case sizeof(uint64_t):
        p_save.clear();
        {
            int64_t p_findP = (uint64_t)std::stoi(p_find);
            if (p_findP < UINT64_MAX && p_findP > 0)
            {
                for (uint64_t i = 0; i < p_lenght; i++)
                {
                    if ((int64_t)p_buffer[i] == p_findP)
                        p_save.push_back(p_offsets);
                    p_offsets++;
                }
            }
        }
        break;
    case sizeof(std::string):
        p_save.clear();
        {
            std::string str;
            for (uint64_t it = 0; it < p_lenght; it++)
            {
                if (p_buffer[it] == p_find[0])
                {
                    for (std::size_t i = 0; i < p_find.size(); i++)
                        str += p_buffer[it + i];

                    if (str == p_find)
                        p_save.push_back(p_offsets);
                }
                p_offsets++;
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
 * @param pid_t p_pid to be attached inside the system
 * @return int
 *
 */
int RemoteProcess::openProcess(pid_t p_pid)
{
    m_proc.pid = p_pid;
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
 * @brief Destroy the Remote Process:: Remote Process object
 *
 */
RemoteProcess::~RemoteProcess()
{
    close(m_proc.fd);
}

/**
 * @brief read memory using pread
 *
 * @param p_start start offset for read mem
 * @param p_stop offset for stop read start + size
 * @param p_data where will the reading be stored in memory
 * @return int  if read sucess return READ_SUCCESS else READ_FAIL
 */
int RemoteProcess::readMem(off_t p_start, off_t p_stop, Data *p_data)
{
    if (m_status == OPEN_FAIL)
        return READ_FAIL;

    size_t bsize = p_stop - p_start;
    if(pread(m_proc.fd, p_data->m_buff, bsize, p_start) == -1)
        return READ_FAIL;

    return READ_SUCCESS;
}

/**
 * @brief Write into a remote process memory
 *
 * @param p_start p_offsets to be written into
 * @param p_data class with the target data to be write
 * @return int
 *
 * If the remote memory cannot be written, it will returns WRITE_FAIL, otherwise WRITE_SUCESS. The error cause can be found at the
 * errno variable, which is set by ptrace.
 */
int RemoteProcess::writeMem(off_t p_start, Data *p_data)
{
    if (m_status == OPEN_FAIL)
        return WRITE_FAIL;

    ssize_t write = pwrite(m_proc.fd, reinterpret_cast<const void *>(p_data->m_buff), p_data->m_size, p_start);
    if (write == -1)
        return WRITE_FAIL;

    return WRITE_SUCCESS;
}

/**
 * @brief find values ​​in memory
 *
 * @param p_start offset for start
 * @param p_length size for stop read
 * @param p_type type for search value
 * @param p_find what find
 * @param p_offsets store the offsets
 * @return int
 */
int RemoteProcess::findMem(off_t p_start, uint64_t p_length, uint8_t p_type, std::string p_find, std::vector<off_t> &p_offsets)
{
    if (m_status == OPEN_FAIL)
    {
        throw std::runtime_error("Error not open file " + m_proc.dirmem);
        return OPEN_FAIL;
    }
    else if (!m_hasProcMem)
    {
        try
        {
            char *p_buffer = (char*)fast.req(p_length);
            if (pread(m_proc.fd, p_buffer, p_length, p_start) == -1)
                return READ_FAIL;
            else
                analyseMemory(p_buffer, p_find, p_start, p_type, p_length, p_offsets);
        }
        catch (std::exception &error)
        {
            throw std::runtime_error(error.what());
        }
    }
    else
        throw std::runtime_error("Not supported p_find memory, directory 'proc/" + std::to_string(m_proc.pid) + "/mem' not found, \n maProc not support for read mem with ptrace");

    return READ_SUCCESS;
}

/**
 * @brief stop process
 *
 * @param p_enable if stopped is return cont set true parameter
 */
void RemoteProcess::stopPid(bool p_enable)
{
    if (p_enable == true && m_proc.pid != 0)
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
// to move the p_buffer for writing to memory and reading
//

Data::Data(uint p_size)
{
    m_size = p_size;
    m_buff = new uint8_t[m_size];
    externalRef = false;
    m_curr = 0;
}

Data::Data(uint8_t *p_buff, uint p_size)
{
    externalRef = true;
    m_buff = p_buff;
    m_size = p_size;
    m_curr = 0;
}

Data::~Data()
{
    if (!externalRef)
        delete[] m_buff;
}

void Data::write(uint8_t p_b)
{
    if (m_curr++ == m_size)
        return;

    m_buff[m_curr] = p_b;
}

uint8_t *Data::read()
{
    return (uint8_t *)m_buff;
}

void Data::clear()
{
    memset(m_buff, 0, m_size);
}