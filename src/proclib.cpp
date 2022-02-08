#include "include/proclib.hpp"

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

    if (ptrace(PTRACE_ATTACH, proc.pid, NULL, NULL) < 0)
    {
        return status;
        // error is set in errno
    }

    // wait to the attach ends

    int wstatus;
    waitpid(proc.pid, &wstatus, 0);

    if (WIFSTOPPED(wstatus))
    {
        // process has been attached and stopped, continue normally
        ptrace(PTRACE_CONT, proc.pid, NULL, NULL);
        status = OPEN_SUCCESS;
        return status; // success :)
    }

    return status;
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
    if (status != OPEN_SUCCESS)
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
 * @param stop offset to top looking
 * @param data if found, it will be stored in data
 * @return int 
 */
int RemoteProcess::findMem(off_t start, off_t stop, Data *data)
{
    if (status != OPEN_SUCCESS)
        return NOT_FOUND;

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