#pragma once

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <string>
#include <iostream>

#include "datastructs/erros.hpp"

#define BUFFER_MAX_ANALYTICS 65536

class Data
{
public:
    Data(uint __size);
    virtual ~Data();
    void write(uint8_t b);
    uint8_t read();
    void clear();
    uint size;
    uint8_t *buff;

private:
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

protected:
    RemoteProcess(){};
    virtual ~RemoteProcess() {}

    int openProcess(pid_t __pid);
    int readMem(off_t start, Data *data);
    int writeMem(off_t start, Data *data);
    int findMem(off_t start, Data *data, std::string find);
};