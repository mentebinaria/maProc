#ifndef MAPPED_H
#define MAPPED_H

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "structs/status.hpp"
#include "structs/addr.hpp"

#define off_t long
#define PROC "/proc/"

#if __x86_64__
#define key_t uint64_t;
#else
#define key_t uint32_t;
#endif

class mapper_memory
{

private: 
    std::string  maps_buf, status_buf;
    unsigned int pid, pid_max;

    std::fstream MAPS_FS;
    std::fstream STATUS_FS;

    Address_info ADDR_INFO;
    Status_info STATS_INFO;

    bool mem_write(off_t __addr, void *__val);
    void mem_read(off_t __on, off_t __off);
    
    void split_mem_address(std::string);
    void split_status_process();

public:
    mapper_memory() throw();
    ~mapper_memory();

    int map_pid(unsigned int __pid);
    bool map_mem(std::string __mem); 

    bool map_write();
    bool map_read();

    off_t get_addrOn() const;
    off_t get_addrOff() const;

    size_t get_sizeAddress();
};

#endif // MAPPED_H