#ifndef MAPPED_H
#define MAPPED_H

#include <iostream>
#include <fstream>
#include <unordered_map>

#define off_t long
#define PROC "/proc/"
#define MAPS "/maps"
#define STATUS "/status"

#if __x86_64__
#define key_t uint64_t;
#else
#define key_t uint32_t;
#endif

struct Address_info
{
    off_t addr_on;
    off_t addr_off;
};

class mapper_memory
{

private: std::string pid, maps_buf, status_buf;
   
    
    std::fstream MAPS_FS;
    std::fstream STATUS_FS;
    Address_info ADDR_INFO;

    bool mem_write(off_t __addr, void *__val);
    void mem_read(std::string __on, std::string __off);
    void split_mem_address(std::string __line);

public:
    mapper_memory();
    ~mapper_memory();

    int map_pid(std::string __pid);
    bool map_mem(std::string __mem);

    bool map_write();
    bool map_read();

    off_t get_addr_on() const;
    off_t get_addr_off() const;

    size_t get_size_address();
};

#endif // MAPPED_H