#pragma once

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <stdexcept>

#include "datastructs/erros.hpp"

#define PROC "/proc/"

class Data
{
public:
    Data(uint);
    Data(uint8_t *, uint);
    virtual ~Data();
    void write(uint8_t);
    uint8_t *read();
    void clear();
    uint m_size;
    uint8_t *m_buff;

private:
    bool externalRef;
    uint m_curr;
};

class RemoteProcess
{
    protected:
        struct Process
        {
            pid_t pid;
            std::string dirmem;
            int fd;

        } m_proc;

        RemoteProcess();
        virtual ~RemoteProcess();

        void analyseMemory(char *, std::string, off_t,
                    uint8_t, uint64_t, std::vector<off_t> &);
        int openProcess(pid_t);
        int readMem(off_t, off_t, Data *);
        int writeMem(off_t, Data *);
        int findMem(off_t, uint64_t, uint8_t, std::string, std::vector<off_t> &);
        void stopPid(bool);
        void killPid();

    private:
        int m_status;
        bool m_hasProcMem;
};