#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "proclib.hpp"
#include "include/datastructs/utils.hpp"

#define PROC "/proc/"
#define MAPS "/maps"


class FileDescriptor
{
public:
    FileDescriptor();
    ~FileDescriptor();
    int readFS(std::string __name, std::string &__buffer,
               long __nblock, bool __blockn2);
};

class Pmap : protected RemoteProcess
{
private:
    FileDescriptor FS;
    std::string maps_buf;

    void split_mem_address(std::string __foo);
    struct Infos
    {
        off_t addr_on;
        off_t addr_off;
        pid_t pid;
        pid_t pid_max;

    } infos;

public:
    Pmap();
    ~Pmap();

    int map_pid(pid_t __pid);
    bool map_mem(std::string __mem);

    bool map_write(off_t __addr, unsigned int __type);
    bool map_read(off_t __addr, unsigned int __type);
    int map_find();

    off_t get_addrOn() const;
    off_t get_addrOff() const;
    off_t get_sizeAddress();
    std::string get_utilsPid(int __utils);
};
