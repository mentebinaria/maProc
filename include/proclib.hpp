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
    Data(uint8_t *m_buff, uint m_size);
    virtual ~Data();
    void write(uint8_t b);
    uint8_t *read();
    void clear();
    uint m_size;
    uint8_t *m_buff;
    uint m_ccurr;
    bool externalRef;

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
        pid_t pid;
        std::string dirmem;
        int fd;

    } m_proc;

    RemoteProcess();
    virtual ~RemoteProcess();

    int Analyse(char *, std::string, off_t,
                uint8_t, uint64_t, std::vector<off_t> &);
    int openProcess(pid_t);
    int readMem(off_t, off_t, Data *);
    int writeMem(off_t, Data *);
    int findMem(off_t, uint64_t, uint8_t, std::string, std::vector<off_t> &);
    void stopPid(bool);
    void killPid();
};