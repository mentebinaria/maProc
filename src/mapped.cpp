#include <dirent.h>
#include <sys/ptrace.h>

#include "include/mapped.hpp"

#define OPEN_FILE(__fs, __name) \
    __fs.open(__name);

#define CLOSE_FILE(__fs) __fs.close()

#define VERIFY_OPEN_CLOSE(__fs) \
    if (__fs.is_open())         \
        __fs.close();

#define CLEAR_STRING(__str) __str.clear();

#define SAVE_BUF_FILE(__fs, __load)           \
    {                                         \
        std::string cache;                    \
        CLEAR_STRING(cache);                  \
        while (getline(__fs, cache))          \
            __load.operator+=(cache += '\0'); \
        CLEAR_STRING(cache);                  \
    }

#define PID_MAX                      \
    {                                \
        std::fstream fs;             \
        std::string buffer;          \
        OPEN_FILE(fs, LIMITE_PID);   \
        SAVE_BUF_FILE(fs, buffer);   \
        pid_max = std::stoi(buffer); \
        CLEAR_STRING(buffer)         \
        CLOSE_FILE(fs);              \
    }

/**
 * @brief function verify pid for passed in user
 * @param unsigned int __pid pass pid for verification is exist
 * @param unsigned int __max pass to max pid for verification for not ultrapassing
 * @return bool
 *
 * if process exist for linux return true else false
 */
static int verify_pid(unsigned int __pid, unsigned int __max) throw()
{
    bool status_exit = 0;

    if (__pid <= 0 || __pid > __max)
    {
        status_exit = -3;
        throw std::runtime_error("Pid " + std::to_string(__pid) + " passed is not valid, valid pid less 0 or larger " + std::to_string(__max));
    }

    std::string proc = PROC + std::to_string(__pid);
    DIR *dir = opendir(proc.c_str());

    if (dir == NULL)
    {
        status_exit = -1;
        throw std::runtime_error("Pid not found, verify to pid and pass pid valid");
    }

    closedir(dir);
    return status_exit;
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
void mapper_memory::mem_read(off_t __on, off_t __off)
{
    ptrace(PTRACE_ATTACH, pid, 0, 0); // ptrace attach for reading process

    for (off_t i = __on; i <= __off; i++)
    {
        // off_t data = ptrace(PTRACE_PEEKTEXT, std::stoi(__pid), i, 0);
    }
}

/**
 * @brief will do a get the addresses from the line
 * @param std::string __line which line to pass to get the addresses
 * @param Address_info *addr [opcional] save address in struct create struct Address_info addr;

 * @return void
 */
void mapper_memory::split_mem_address(std::string __line, Address_info *addr)
{
    std::string addr_on;
    std::string addr_off;

    if (addr_on.size() == 0 && addr_off.size() == 0)
    {
        CLEAR_STRING(addr_on)
        CLEAR_STRING(addr_off)
    }

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

    off_t addr_on_long = std::stoul(addr_on, nullptr, 16);
    off_t addr_off_long = std::stoul(addr_off, nullptr, 16);

    if (addr_on.size() == 0 || addr_on.size() == 0)
        std::runtime_error("Error not found address_on and address_off");
    else if (addr != nullptr)
    {
        addr->addr_on = addr_on_long;
        addr->addr_off = addr_off_long;
    }

    ADDR_INFO.addr_on = addr_on_long;
    ADDR_INFO.addr_off = addr_off_long;

    CLEAR_STRING(__line)
}

/**
 * @brief get status process
 *
 */
void mapper_memory::split_status_process()
{
}

/**
 *  @brief Constructor
 */
mapper_memory::mapper_memory()
{
    PID_MAX;
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
 *  if pid not found return -1
 *  if process not len -2
 *  else is true return 0
 */
int mapper_memory::map_pid(unsigned int __pid)
{
    VERIFY_OPEN_CLOSE(MAPS_FS)
    VERIFY_OPEN_CLOSE(STATUS_FS)

    pid = __pid;

    std::string pid_str = std::to_string(pid);
    std::string maps = PROC + pid_str + MAPS;
    std::string status = PROC + pid_str + STATUS;

    int status_pid = verify_pid(pid, pid_max);

    RemoteProcess::openProcess(pid);

    if (status_pid == 0)
    {
        CLEAR_STRING(maps_buf);
        CLEAR_STRING(status_buf);

        OPEN_FILE(MAPS_FS, maps);
        OPEN_FILE(STATUS_FS, status);

        SAVE_BUF_FILE(MAPS_FS, maps_buf);
        SAVE_BUF_FILE(STATUS_FS, status_buf);

        if (maps_buf.size() == 0 || status_buf.size() == 0)
        {
            throw std::runtime_error("Not possible len infos to process");
            status_pid = -2;
        }
    }

    return status_pid;
}

/**
 * @brief verify mem if process usage
 * @param std::string __mem memory to be mapped will check if it exists and map such memory
 * @param Address_info *addr [opcional] save address in struct create struct Address_info addr;
 * @return bool
 *
 * if the use of memory is `not` being used it returns true or returns false
 * if the memory is not being used by the process, it will map right away
 * that the memory is being used
 */
bool mapper_memory::map_mem(std::string __mem, Address_info *addr)
{
    bool status_exit = true;

    std::string found;
    search_line(__mem, found, maps_buf);

    if (found.size() == 0)
    {
        status_exit = false;
        throw std::runtime_error("Process not using memory " + __mem);
    }
    else
        split_mem_address(found, addr);

    return status_exit;
}

/**
 * @brief get address on
 * @return off_t
 *
 * in address start for mem process
 */
off_t mapper_memory::get_addrOn() const
{
    return ADDR_INFO.addr_on;
}

/**
 * @brief get address off
 * @return off_t
 *
 * in address stop for mem process
 */
off_t mapper_memory::get_addrOff() const
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