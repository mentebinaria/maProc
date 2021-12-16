#include <dirent.h>
#include <sys/ptrace.h>
#include <list>

#include "include/mapped.hpp"

#define OPEN_FILE(__fs, __name) \
    __fs.open(__name);

#define CLOSE_FILE(__fs) __fs.close()

#define VERIFY_OPEN_CLOSE(__fs) \
    if (__fs.is_open())         \
        __fs.close();

#define CLEAR_STRING(__str) __str.clear();

/**
 * @brief function verify pid for passed in user
 * @param std::string __pid pass pid for verification
 * @return bool
 *
 * if process exist for linux return true else false
 */
static bool verify_pid(std::string __pid)
{
    std::string proc = PROC + __pid;
    DIR *dir = opendir(proc.c_str());
    closedir(dir);
    return (dir) ? true : false;
}

/**
 *  @brief Function static for search to passing
 *  @param std::string __find which string to search
 *  @param std::string &__load lading search buffer
 *  @param std::string &__buffer string to searching
 *  @return void
 *
 *  @note this function will get the line completely by
 *  traversing the string backwards, will clear the string to be loaded the line
 */
static void search_line(std::string __find, std::string &__load, std::string &__buffer)
{
    CLEAR_STRING(__load)

    std::size_t find_pos = __buffer.find(__find);

    if (find_pos != std::string::npos)
    {
        for (; __buffer[find_pos] != '\0'; find_pos--)
            __load += __buffer[find_pos];
    }

    std::string format;
    for (std::size_t i = __load.size(); i > 0; i--)
        format += __load[i];

    __load = format;

    CLEAR_STRING(format);
}

/**
 *  @brief Mem write passing address for to modify
 *  @param off_t __addr pass address for modify
 *  @param void* __val pass value for modify
 *  @return bool
 */
bool mapper_memory::mem_write(off_t __addr, void *__val)
{
    bool status_exit = true;

    return status_exit;
}

/**
 *  @brief Reading mem for address in process to preference heap or stack
 *  @param std::string __on address for init reading memory
 *  @param std::string __off for end address mapped
 *  @return void
 */
void mapper_memory::mem_read(std::string __on, std::string __off)
{
    ptrace(PTRACE_ATTACH, std::stoi(pid), 0, 0); // ptrace attach for reading process

    off_t on_long = std::stoul(__on, nullptr, 16);
    off_t off_long = std::stoul(__off, nullptr, 16);

    if (on_long == 0)
        return;

    for (off_t i = on_long; i <= off_long; i++)
    {
        // off_t data = ptrace(PTRACE_PEEKTEXT, std::stoi(__pid), i, 0);
    }
}

/**
 *  @brief Constructor
 */
mapper_memory::mapper_memory()
{
}

/**
 *  @brief Destructor
 */
mapper_memory::~mapper_memory()
{
    CLOSE_FILE(MAPS_FS);
    CLOSE_FILE(STATUS_FS);
}

/**
 *  @brief  Mapper process passing pid
 *  @param  __pid pid of the process to be mapped
 *  @return int
 *
 * if pid not found return -1 else return 1
 * if process not len return -2
 */
int mapper_memory::map_pid(std::string __pid)
{
    pid = __pid;
    std::string maps = PROC + pid + MAPS;
    std::string status = PROC + pid + STATUS;
    std::string buffer;

    VERIFY_OPEN_CLOSE(MAPS_FS)
    VERIFY_OPEN_CLOSE(STATUS_FS)

    int status_exit = 1;

    if (!verify_pid(pid))
    {
        status_exit = -1;
        pid = nullptr;
    }
    else
    {
        CLEAR_STRING(maps_buf);
        CLEAR_STRING(status_buf);

        status_exit = 1;

        OPEN_FILE(MAPS_FS, maps)
        OPEN_FILE(STATUS_FS, status)

        while (getline(MAPS_FS, buffer))
            maps_buf += buffer += '\0';

        if (maps_buf.size() == 0)
            status_exit = -2;

        CLEAR_STRING(buffer)

        while (getline(STATUS_FS, buffer))
            status_buf += buffer += '\0';

        CLEAR_STRING(buffer)
    }

    return status_exit;
}

/**
 * @brief verify mem if process usage
 * @param std::string __mem memory to be mapped will check if it exists and map such memory
 * @return bool
 *
 * if the use of memory is `not` being used it returns true or returns false
 * if the memory is not being used by the process, it will map right away
 * that the memory is being used
 */
bool mapper_memory::map_mem(std::string __mem)
{
    bool status_exit = true;

    std::string found;
    search_line(__mem, found, maps_buf);

    if (found.size() == 0)
        status_exit = false;
    else
        split_mem_address(found);

    return status_exit;
}

/**
 * @brief will do a get the addresses from the line
 * @param std::string __line which line to pass to get the addresses
 * @return void
 */
void mapper_memory::split_mem_address(std::string __line)
{
    std::string addr_on;
    std::string addr_off;

    CLEAR_STRING(addr_on)
    CLEAR_STRING(addr_off)

    // get address start
    for (std::size_t i = 1; i <= __line.size(); i++)
    {
        if (__line[i] == '-')
            break;
        addr_on += __line[i];
    }

    // get address end
    for (std::size_t i = addr_on.size() + 2; i <= __line.size(); i++)
    {
        addr_off += __line[i];
        if (__line[i] == ' ')
            break;
    }

    if (addr_on.size() == 0 || addr_on.size() == 0)
        std::exit(EXIT_FAILURE);

    ADDR_INFO.addr_on  = std::stoul(addr_on, nullptr, 16);
    ADDR_INFO.addr_off = std::stoul(addr_off, nullptr, 16);

    CLEAR_STRING(__line)
}

/**
 * @brief get address on
 * @return off_t
 *
 * in address start for mem process
 */
off_t mapper_memory::get_addr_on() const
{
    return ADDR_INFO.addr_on;
}

/**
 * @brief get address off
 * @return off_t
 *
 * in address stop for mem process
 */
off_t mapper_memory::get_addr_off() const
{
    return ADDR_INFO.addr_off;
}

/**
 * @brief get address size mapped
 * @return size_t
 */
size_t mapper_memory::get_size_address()
{
    off_t address_size = ADDR_INFO.addr_off - ADDR_INFO.addr_on;
    return address_size;
}