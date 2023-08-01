#pragma once

#include <string>
#include <cstdint>

/**
 * @brief clean string
 * @note using for 'free' string to buffer
 */
#define CLEAR_STRING(__str) __str.clear();

/**
 * @brief Get pid max your system support
 * @note this len file /proc/sys/kernel/pid_max
 */
#define PID_MAX                                          \
    std::string buffer;                                  \
    m_FS.readFS("/proc/sys/kernel/pid_max", buffer, 10); \
    m_infos.pid_max = std::stoi(buffer);                 \
    CLEAR_STRING(buffer);

#define MAPS "/maps"
#define BUFFER_READ_UTILS 15

struct Maps
{
    off_t addr_on;
    off_t addr_off;
    uint64_t size_map;
    off_t offset;
    std::string flags;
    std::string pathname;
};

struct Infos
{
    pid_t pid;
    pid_t pid_max;

};

/**
 * @brief get utils info pid
 * 
 */
enum 
{
    NAME = 1,
    WCHAN,
    SESSIONID,
    CMDLINE,
    LOGINUID,
    SIZEBIN,
    BLOCKSIZEBIN,
    EXEDIR,
    COUNT,
};

enum 
{
    HOSTNAME = COUNT,
    OSREALESE,
    VERSION,
    TYPE,
};