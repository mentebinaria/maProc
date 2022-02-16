#include <dirent.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>

#include "include/pmap.hpp"
#include "include/datastructs/erros.hpp"

/**
 * @brief function verify pid for passed in user
 * @param __pid pass pid for verification is exist
 * @param __max pass to max pid for verification for not ultrapassing pid limit
 * @return int
 *
 * if process exist for linux return true else false
 */
static int verify_pid(pid_t __pid, pid_t __max)
{
    int status_exit = PID_SUCCESS;

    if (__pid <= 0 || __pid > __max)
    {
        status_exit = PID_OVERDRIVE;
        throw std::runtime_error("Pid " + std::to_string(__pid) + " passed is not valid, valid pid not less 0 or larger " + std::to_string(__max));
    }
    else
    {
        std::string proc = PROC + std::to_string(__pid);
        DIR *dir = opendir(proc.c_str());

        if (dir == nullptr)
        {
            status_exit = PID_NOT_FOUND;
            throw std::runtime_error("Pid not found, verify to pid and pass pid valid");
        }
        else
            closedir(dir);
    }

    return status_exit;
}

/**
 *  @brief Function static for search will get the line completely by
 *  traversing the string backwards, will clear the string to be loaded the line
 *  @param __find which string to search
 *  @param &__load lading search buffer
 *  @param &__buffer string to searching
 *  @return void
 */
static void search_line(std::string __find, std::string &__load, std::string &__buffer)
{
    std::size_t find_pos = __buffer.find(__find);

    if (find_pos != std::string::npos)
    {
        for (; __buffer[find_pos] != '\n'; find_pos--)
            __load += __buffer[find_pos];
    }

    std::string format;
    for (std::size_t i = __load.size(); i > 0; i--)
        format += __load[i];

    __load = format;

    CLEAR_STRING(format);
}

/**
 * @brief will do a get the addresses from the line
 * @param __line which line to pass to get the addresses

 * @return void
 */
void Pmap::split_mem_address(std::string __foo)
{
    // get address start
    std::string addr_on;
    for (std::size_t i = 1; i <= __foo.size(); i++)
    {
        if (__foo[i] == '-')
            break;
        addr_on += __foo[i];
    }

    // get address end
    std::string addr_off;
    for (std::size_t i = addr_on.size() + 2; i <= __foo.size(); i++)
    {
        addr_off += __foo[i];
        if (__foo[i] == ' ')
            break;
    }

    // convert string to off_t
    off_t addr_off_long = static_cast<off_t>(std::stoul(addr_off, nullptr, 16));
    off_t addr_on_long = static_cast<off_t>(std::stoul(addr_on, nullptr, 16));

    if (addr_on.size() == 0 || addr_on.size() == 0)
    {
        throw std::runtime_error("Error not split address_on and address_off");
        return;
    }
    else
    {
        infos.addr_on = addr_on_long;
        infos.addr_off = addr_off_long;
    }
    CLEAR_STRING(__foo)
}

/**
 *  @brief Constructor
 */
Pmap::Pmap()
{
    infos.pid_max = 0;
    infos.pid = 0;
    infos.addr_on = 0;
    infos.addr_off = 0;
    PID_MAX
}

/**
 *  @brief Destructor
 */
Pmap::~Pmap()
{
    CLEAR_STRING(maps_buf);
}

/**
 *  @brief  Mapper process passing pid
 *  @param  __pid pid of the process to be mapped
 *  @return int
 *
 *  pid not found return PID_NOT_FOUND
 *  if the pid exists ira return PID_SUCCESS
 */
int Pmap::map_pid(pid_t __pid)
{
    infos.pid = __pid;

    int status_exit = verify_pid(infos.pid, infos.pid_max);
    RemoteProcess::openProcess(infos.pid);

    std::string pid_str = std::to_string(infos.pid);
    std::string fmaps = PROC + pid_str + MAPS;

    FS.readFS(fmaps, maps_buf, 2048, true);

    if (maps_buf.size() == 0)
    {
        status_exit = PID_NOT_READ;
        throw std::runtime_error("Looks like proc/" + std::to_string(infos.pid) + "/maps is empty, I do a check in the past process");
    }

    return status_exit;
}

/**
 * @brief verify mem if process usage
 * @param __mem memory to be mapped will check if it exists and map such memory
 * @return bool
 *
 * if the use of memory is `not` being used it returns true or returns false
 * if the memory is not being used by the process, it will map right away
 * that the memory is being used
 */
bool Pmap::map_mem(std::string __mem)
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
        split_mem_address(found);

    return status_exit;
}

/**
 *  @brief Mem write passing address for to modify
 *  @param __addr pass address for modify
 *  @param __val pass value for modify
 *  @return bool
 */
bool Pmap::map_write(off_t __addr, unsigned int __type)
{
    Data data(__type);
    RemoteProcess::readMem(__addr, &data);
    return true;
}

/**
 *  @brief Reading mem for address in process to preference heap or stack
 *  @param __on address for init reading memory
 *  @param __off for end address mapped
 *  @return void
 */
bool Pmap::map_read(off_t __addr, unsigned int __type, Data &__data)
{
    bool status_exit = false;
    Data data(__type);

    if (RemoteProcess::readMem(__addr, &data) != READ_FAIL)
    {
        status_exit = true;
        __data = data;
    }

    return status_exit;
}

/**
 * @brief
 *
 * @return off_t address in find type
 */
off_t Pmap::map_find(off_t __addr, std::string __find, uint8_t __type)
{
    Data data(__type);

    return RemoteProcess::findMem(__addr, &data, __find);
}

/**
 * @brief get address on
 * @return off_t
 *
 * in address start for mem process
 */
off_t Pmap::get_addrOn() const
{
    return infos.addr_on;
}

/**
 * @brief get address off
 * @return off_t
 *
 * in address stop for mem process
 */
off_t Pmap::get_addrOff() const
{
    return infos.addr_off;
}

/**
 * @brief get address size mapped
 * @return size_t
 */
off_t Pmap::get_sizeAddress()
{
    return infos.addr_off - infos.addr_on;
}

/**
 * @brief take the pid utility as an example, the name of the pid,
 * just pass it as a NAME parameter,
 * which will return the process name
 *
 * @param __utils parameter used to get the pid information
 * @return std::string
 */
std::string Pmap::get_utilsPid(uint8_t __utils)
{
    std::string buffer = " ";
    std::string pid_str = std::to_string(infos.pid);
    std::string name = PROC + pid_str;
    struct stat stats;

    switch (__utils)
    {
    case NAME:
        name += "/comm";
        FS.readFS(name, buffer, 10);
        break;

    case WCHAN:
        name += "/wchan";
        FS.readFS(name, buffer, 10);
        break;

    case SESSIONID:
        name += "/sessionid";
        FS.readFS(name, buffer, 10);
        break;

    case CMDLINE:
        name += "/cmdline";
        FS.readFS(name, buffer, 10);
        break;

    case LOGINUID:
        name += "/loginuid";
        FS.readFS(name, buffer, 10);
        break;

    case SIZEBIN:
        name += "/exe";
        if (stat(name.data(), &stats) == 0)
            buffer = std::to_string(stats.st_size);
        break;

    case BLOCKSIZEBIN:
        name += "/exe";
        if (stat(name.data(), &stats) == 0)
            buffer = std::to_string(stats.st_blksize);
        break;

    default:
        throw std::runtime_error("It was not possible to make the get at this value " + __utils);
    }

    return buffer;
}