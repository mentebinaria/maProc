#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "proclib.hpp"
#include "datastructs/utils.hpp"
#include "datastructs/utils_pid.hpp"
#include "include/filedescriptor.hpp"

#define PROC "/proc/"
#define MAPS "/maps"

/**
 * @brief manipulation of process 
 * 
 */
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
    bool map_read(off_t __addr, unsigned int __type, Data &__data);
    off_t  map_find(off_t __addr, std::string __find, uint8_t __type);

    off_t get_addrOn() const;
    off_t get_addrOff() const;
    off_t get_sizeAddress();
    std::string get_utilsPid(uint8_t __utils);
};