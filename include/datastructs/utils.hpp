#pragma once

/**
 * @brief clean string
 * @note using for 'free' string to buffer
 */
#define CLEAR_STRING(__str) __str.clear();

/**
 * @brief Get pid max your system support
 * @note this len file /proc/sys/kernel/pid_max
 */
#define PID_MAX                                        \
    std::string buffer;                                \
    FS.SaveBuffer("/proc/sys/kernel/pid_max", buffer); \
    pid_max = std::stoi(buffer);                       \
    CLEAR_STRING(buffer)