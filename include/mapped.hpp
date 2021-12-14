#ifndef MAPPED_H
#define MAPPED_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <unordered_map>

#define off_t long
#define PROC "/proc/"
#define MAPS "/maps"

#if __x86_64__
    #define key_t uint64_t;
#else 
    #define key_t uint32_t;
#endif


class mapper_memory
{

private:
    std::string pid, mem_line, maps_buf, addr_on, addr_off;
    std::fstream fs; 

    bool mem_write(off_t __addr, void* __val);
    void mem_read(std::string __on, std::string __off);
    void mem_address();

public:
    mapper_memory();
    ~mapper_memory();

    int map_pid(std::string __pid);
    bool map_mem(std::string __mem);

    bool map_write();
    bool map_read();

    std::string get_addr_on() const;
    std::string get_addr_off() const;

    size_t get_size_address();
};

#endif // MAPPED_H