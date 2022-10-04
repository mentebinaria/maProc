#pragma once

#include <fstream>
#include <unordered_map>

#include "proclib.hpp"
#include "src/include/data/map/structs.hpp"
#include "src/include/fd/filedescriptor.hpp"


/**
 * @brief manipulation of process
 *
 */
class Pmap : protected RemoteProcess
{
public:
    Pmap();
    ~Pmap();

    int map_pid(pid_t );
    bool map_mem(const std::string &);
    
    bool map_write(off_t, void *, uint);
    bool map_read(off_t, uint, Data &);
    int map_find(off_t, uint64_t, std::string, uint8_t, std::vector<off_t> &);
    void map_stop(bool = true);
    void map_kill();
    
    off_t get_addrOn() const;
    off_t get_addrOff() const;
    uint64_t get_sizeAddress();
    std::string get_Flags();
    
    const std::unordered_map<std::string, Maps>& get_Maps() const;
    std::string get_utilsPid(uint8_t);
    
private:
    Maps m_maps;
    Infos m_infos;
    FileDescriptor m_FS;
    std::string m_maps_buf;
    
    void split_mem_address(std::string &);
    void split_maps();

    std::unordered_map<std::string, Maps> m_unmap;
};