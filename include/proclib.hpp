#pragma once

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <vector>

#include "datastructs/erros.hpp"

#define PROC "/proc/"

class Data
{
public:
    Data(uint __size);
    virtual ~Data();
    void write(uint8_t b);
    uint8_t read();
    void clear();
    uint m_size;
    uint8_t *m_buff;

private:
    uint m_curr;
};

class RemoteProcess
{
private:
    int m_status;
    bool m_hasProcMem;

protected:
    struct Process
    {
        off_t baseAddr;
        pid_t pid;

    } m_proc;

    RemoteProcess();
    virtual ~RemoteProcess();

    int Analyse(char *__buffer, std::string __find, off_t __offset,
                uint8_t __type, uint64_t lenght, std::vector<off_t> &offset);
    int openProcess(pid_t __pid);
    int readMem(off_t start, Data *data);
    int writeMem(off_t start, Data *data);
    int findMem(off_t start, uint64_t length, uint8_t type, std::string find, std::vector<off_t> &offsets);
    void closePid();
};