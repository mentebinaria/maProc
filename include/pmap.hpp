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

    void split_mem_address(std::string &);

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

    bool map_write(off_t, void *, uint);
    bool map_read(off_t, uint, Data &);
    int map_find(off_t, uint64_t, std::string, uint8_t, std::vector<off_t> &);
    void map_stop(bool = true);
    void map_kill();

    off_t get_addrOn() const;
    off_t get_addrOff() const;
    uint64_t get_sizeAddress();
    std::string get_Flags();
    std::string get_utilsPid(uint8_t);
};