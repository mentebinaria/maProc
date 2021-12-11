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
    std::string pid, mem, addr_on, addr_off;

    //std::pair<bool, std::string> mapper_line(std::string __find, std::string __pid);

    bool mem_write(off_t __addr, void* __val);
    void mem_read(std::string __pid, std::string __on, std::string __off);
    void mem_address();

public:
    mapper_memory();
    ~mapper_memory();

    bool verify_pid(std::string __pid);
    bool verify_mem(std::string __mem);

    bool map_write();
    bool map_read();

    std::string get_addr_on();
    std::string get_addr_off();

    size_t get_size_address();
};

#endif // MAPPED_H