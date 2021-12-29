#pragma once

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <string>
#include <iostream>

#include "structs/erros.hpp"

struct Process
{
    pid_t pid;
    std::string name;
    off_t stackStart;
    off_t heapStart;
};

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
    Process proc;
    int status;

protected:
    RemoteProcess(){};
    virtual ~RemoteProcess() {}

    int openProcess(pid_t __pid);
    int readMem(off_t start, Data *data);
    int writeMem(off_t start, Data *data);
};