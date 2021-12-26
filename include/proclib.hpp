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


class Data {
public:
    Data(uint __size);
    virtual ~Data();
    void write(uint8_t b);
    uint8_t read();

    uint size;
private:
    uint8_t* buff;
    uint curr;
};

enum {
    OPEN_SUCCESS,
    OPEN_FAIL,
    READ_SUCCESS,
    READ_FAIL,
    WRITE_FAIL,
    WRITE_SUCCESS
};

class RemoteProcess {
public:
    RemoteProcess() {};
    virtual ~RemoteProcess() {}

    int openProcess(pid_t __pid);
    int readMem(off_t start, Data* data);
    int writeMem(off_t start, Data* data);

private:
    Process proc;
    int status;
};