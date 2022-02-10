#pragma once

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string>
#include <iostream>

#include "datastructs/erros.hpp"


class Data
{
public:
    Data(uint __size);
    virtual ~Data();
    void write(uint8_t b);
    uint8_t read();
    uint size;

private:
    uint8_t *buff;
    uint curr;
};

class RemoteProcess
{
private:
    struct Process
    {
        off_t baseAddr;
        pid_t pid;
    } proc;
    
    int status;
    bool hasProcMem;
    
protected:
    RemoteProcess();
    virtual ~RemoteProcess();

    int openProcess(pid_t __pid);
    int readMem(off_t start, Data *data);
    int writeMem(off_t start, Data *data);
    int findMem(off_t start, Data *data, std::string find);
};