#pragma once

/**
 * @brief get utils info pid
 * 
 */
enum PID_INFO
{
    NAME = 1,
    WCHAN,
    SESSIONID,
    CMDLINE,
    LOGINUID,
    SIZEBIN,
    BLOCKSIZEBIN,
    COUNT,
};

enum SYS_INFO
{
    HOSTNAME = COUNT,
    OSREALESE,
    VERSION,
    TYPE,
};