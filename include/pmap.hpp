#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "proclib.hpp"
#include "datastructs/utils.hpp"
#include "datastructs/utils_pid.hpp"
#include "include/filedescriptor.hpp"

#define MAPS "/maps"
#define BUFFER_READ_UTILS 15
/**
 * @brief manipulation of process
 *
 */
class Pmap : protected RemoteProcess
{
private:
    FileDescriptor m_FS;
    std::string m_maps_buf;

    void split_mem_address(std::string &__foo);

    struct Infos
    {
        std::string flags;
        off_t addr_on;
        off_t addr_off;
        pid_t pid;
        pid_t pid_max;

    } m_infos;

public:
    Pmap();
    ~Pmap();

    int map_pid(pid_t __pid);
    bool map_mem(std::string __mem);

    bool map_write(off_t __addr, uint8_t __type);
    bool map_read(off_t __addr, uint8_t __type, Data &__data);
    int map_find(off_t __addr, uint64_t __length, std::string __find, uint8_t __type, std::vector<off_t> &__offsets);
    void map_close();

    off_t get_addrOn() const;
    off_t get_addrOff() const;
    uint64_t get_sizeAddress();
    std::string get_Flags();
    std::string get_utilsPid(uint8_t __utils);
};