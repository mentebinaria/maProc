#ifndef UTILS
#define UTILS

/**
 * @brief open file
 */
#define OPEN_FILE(__fs, __name) \
    __fs.open(__name);

/**
 * @brief close file
 */
#define CLOSE_FILE(__fs) __fs.close()

/**
 * @brief macro using for close file is open true
 */
#define VERIFY_OPEN_CLOSE(__fs) \
    if (__fs.is_open())                \
        CLOSE_FILE(__fs);

/**
 * @brief clean string
 * @note using for 'free' string to buffer
 */
#define CLEAR_STRING(__str) __str.clear();

/**
 * @brief Save buffer your file using to macro
 * @param __fs file for length
 * @param __load is load buffer file
 * @note this macro not open file and not close, verify is file open
 * for len buffer
 */
#define SAVE_BUF_FILE(__fs, __load)           \
    {                                         \
        std::string cache;                    \
        CLEAR_STRING(cache);                  \
        while (getline(__fs, cache))          \
            __load.operator+=(cache.operator+=('\0')); \
        CLEAR_STRING(cache);                  \
    }

/**
 * @brief Get pid max your system support
 * @note this len file /proc/sys/kernel/pid_max
 */
#define PID_MAX()                                                               \
    std::fstream fs;                                                            \
    std::string buffer;                                                         \
    OPEN_FILE(fs, "/proc/sys/kernel/pid_max");                                  \
    if (!fs.is_open())                                                          \
        throw std::runtime_error("Error not possible get pid max your system"); \
    SAVE_BUF_FILE(fs, buffer);                                                  \
    pid_max = std::stoi(buffer);                                                \
    CLEAR_STRING(buffer)                                                        \
    CLOSE_FILE(fs);

#endif // !UTILS