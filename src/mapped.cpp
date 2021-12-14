#include <dirent.h>
#include <sys/ptrace.h>

#include "include/mapped.hpp"

#define OPEN_FILE(__file) \
    fs.open(__file);

#define CLOSE_FILE fs.close()

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
 */
static void split_line(std::string __find, std::string &__load, std::string &__buffer)
{
    __load.clear();

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
    format.clear();
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
    CLOSE_FILE;
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
    if(fs.is_open())
        CLOSE_FILE;
    
    int status_exit;

    if (!verify_pid(__pid))
        status_exit = -1;
    else
    {
        maps_buf.clear();

        status_exit = 1;
        pid = __pid;
        std::string maps = PROC + __pid + MAPS;

        OPEN_FILE(maps)

        std::string buffer;
        while (getline(fs, buffer))
            maps_buf += buffer += '\0';

        if (maps_buf.size() == 0)
            status_exit = -2;

        buffer.clear();
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
    split_line(__mem, found, maps_buf);

    if (found.size() == 0)
        status_exit = false;
    else
    {
        mem_line = found;
        mem_address();
    }

    return status_exit;
}

/**
 * @brief  reading mem for address in process to preference heap or stack
 * @param __mem what memory for map process (stack or heap?)
 * @return void
 */
void mapper_memory::mem_address()
{
    addr_on.clear();
    addr_off.clear();

    // get address start
    for (std::size_t i = 1; i <= mem_line.size(); i++)
    {
        if (mem_line[i] == '-')
            break;
        addr_on += mem_line[i];
    }

    // get address end
    for (std::size_t i = addr_on.size() + 2; i <= mem_line.size(); i++)
    {
        addr_off += mem_line[i];
        if (mem_line[i] == ' ')
            break;
    }

    if (addr_on.size() == 0)
        std::exit(EXIT_FAILURE);

    mem_line.clear();
}

/**
 * @brief get address on
 * @return std::string
 *
 * in address start for mem process
 */
std::string mapper_memory::get_addr_on() const
{
    return addr_on;
}

/**
 * @brief get address off
 * @return std::string
 *
 * in address stop for mem process
 */
std::string mapper_memory::get_addr_off() const
{
    return addr_off;
}

/**
 * @brief get address size mapped
 * @return size_t
 */
size_t mapper_memory::get_size_address()
{
    off_t address_size = std::stoul(addr_off, nullptr, 16) - std::stoul(addr_on, nullptr, 16);
    return address_size;
}