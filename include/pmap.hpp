#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "datastructs/status.hpp"
#include "datastructs/addr.hpp"
#include "proclib.hpp"

#define PROC "/proc/"

#if __x86_64__
#define key_t uint64_t;
#else
#define key_t uint32_t;
#endif

class FileDescriptor
{

public:
    FileDescriptor();
    ~FileDescriptor();
    int readFS(std::string __name, std::string &__buffer,
                    off_t __nblock, bool __blockn2);
};

class Pmap : protected RemoteProcess
{
private:
    std::string maps_buf, status_buf;
    pid_t pid, pid_max;

    FileDescriptor FS;

    Address_info ADDR_INFO;
    Status_info STATS_INFO;

    void split_mem_address(std::string __foo);
    void split_status_process(std::string __foo);

public:
    Pmap();
    ~Pmap();

    int map_pid(pid_t __pid);
    bool map_mem(std::string __mem);

    bool map_write();
    bool map_read();

    off_t get_addrOn() const;
    off_t get_addrOff() const;

    off_t get_sizeAddress();
};
