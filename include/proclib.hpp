#pragma once

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <string>
#include <iostream>

struct Process {
    pid_t pid;
    std::string name;
    off_t stackStart;
    off_t heapStart;
};


class RemoteProcess {
public:
    RemoteProcess() {};
    virtual ~RemoteProcess() {}


    bool openProcess(pid_t __pid);

   // void readMem(off_t start, uint size, void* )

private:
    Process proc;
};