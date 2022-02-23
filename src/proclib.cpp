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
int RemoteProcess::Analyse(char *__buffer, std::string __find, off_t __offset, uint8_t __type, off_t lenght, std::vector<off_t> &offset)
{
    switch (__type)
    {
    case sizeof(char):
        offset.clear();
        for (off_t i = 0; i != lenght; i++)
        {
            __offset++;
            if (__buffer[i] == __find[0])
                offset.push_back(__offset);
        }
        break;
    case sizeof(int):
        offset.clear();
        {
            try
            {
                int find = std::stoi(__find);
                if (find < INT_MAX)
                {
                    for (off_t i = 0; i != lenght; i++)
                    {
                        __offset++;
                        if ((int)__buffer[i] == find)
                            offset.push_back(__offset);
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
        break;

    default:
        break;
    }
    return 0;
}

RemoteProcess::RemoteProcess()
{
    struct stat st;
    hasProcMem = stat("/proc/self/mem", &st) != 0;
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
    proc.pid = __pid;
    status = OPEN_FAIL;

    long requests = ptrace(PTRACE_ATTACH, proc.pid, NULL, NULL);
    if (requests < 0)
        throw std::runtime_error("Not possible attach pid " + std::to_string(proc.pid));

    // wait to the attach ends
    int wstatus;
    waitpid(proc.pid, &wstatus, 0);

    if (WIFSTOPPED(wstatus))
    {
        // process has been attached and stopped, continue normally
        ptrace(PTRACE_CONT, proc.pid, NULL, NULL);
        status = OPEN_SUCCESS;
    }

    return status;
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
    if (status == OPEN_SUCCESS)
        ptrace(PTRACE_DETACH, proc.pid, NULL, NULL);
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
    if (status == OPEN_FAIL)
        return READ_FAIL;

    long b;
    for (uint i = 0; i < data->size; ++i)
    {
        b = ptrace(PTRACE_PEEKDATA, proc.pid, start + i, NULL);
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
    if (status != OPEN_SUCCESS)
        return WRITE_FAIL;

    long s;
    for (uint i = 0; i < data->size; ++i)
    {
        s = ptrace(PTRACE_POKEDATA, proc.pid, start + i, data->read());
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
int RemoteProcess::findMem(off_t start, off_t length, uint8_t type, std::string find, std::vector<off_t> &offsets)
{
    if (status != OPEN_SUCCESS)
        return NOT_FOUND;

    // call back for analytics bin
    if (!hasProcMem)
    {
        std::string name = PROC + std::to_string(proc.pid) + "/mem";
        int fd = open(name.data(), O_RDONLY);
        if (fd > 0)
        {
            char *buffer;
            try
            {
                buffer = new char[length];
                memset(buffer, 0, length); // clear memory for allocation
                ssize_t read = pread(fd, buffer, length, start);
                if (read == -1)
                    return READ_FAIL;

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
        // close and delete buffer mem
    }
    else
    {
        throw std::runtime_error("Not supported find memory, directory /mem not found, \n maProc not support for read mem with ptrace");
        return READ_FAIL;
    }

    return READ_SUCCESS;
}

void RemoteProcess::closePid()
{
    if (status == OPEN_SUCCESS)
        ptrace(PTRACE_DETACH, proc.pid, NULL, NULL);
}

Data::Data(uint __size)
{
    size = __size;
    buff = new uint8_t[size];
    curr = 0;
}

Data::~Data()
{
    delete[] buff;
}

void Data::write(uint8_t b)
{
    if (curr++ > size) // not enough sspace
        return;

    *(buff++) = b;
}

uint8_t Data::read()
{
    curr = curr % size; // cyclic reading
    return buff[curr++];
}

void Data::clear()
{
    memset(buff, 0, size);
}