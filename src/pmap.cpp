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
void Pmap::split_mem_address(std::string &__foo)
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
    std::string flags;
    for (std::size_t i = addr_on.size() + 2; i <= __foo.size(); i++)
    {
        addr_off += __foo[i];
        if (__foo[i] == ' ')
        {
            for (std::size_t i = addr_on.size() + addr_off.size() + 2; i < __foo.size(); i++)
            {
                flags += __foo[i];
                if (__foo[i] == ' ')
                    break;
            }
            break;
        }
    }

    if (addr_on.size() == 0 || addr_off.size() == 0)
    {
        throw std::runtime_error("Error not split address_on and address_off");
        return;
    }

    // convert string to off_t
    off_t addr_off_long = (off_t)std::stoul(addr_off, nullptr, 16);
    off_t addr_on_long = (off_t)std::stoul(addr_on, nullptr, 16);

    m_infos.addr_on = (off_t)addr_on_long, m_infos.addr_off = (off_t)addr_off_long, m_infos.flags = flags;

    CLEAR_STRING(__foo);
    CLEAR_STRING(flags);
}

/**
 *  @brief Constructor
 */
Pmap::Pmap()
{
    m_infos.pid_max = 0;
    m_infos.pid = 0;
    m_infos.addr_on = 0;
    m_infos.addr_off = 0;
    PID_MAX
}

/**
 *  @brief Destructor
 */
Pmap::~Pmap()
{
    CLEAR_STRING(m_maps_buf);
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
    m_infos.pid = __pid;

    int status_exit = verify_pid(m_infos.pid, m_infos.pid_max);
    RemoteProcess::openProcess(m_infos.pid);

    std::string pid_str = std::to_string(m_infos.pid);
    std::string fmaps = PROC + pid_str + MAPS;

    m_FS.readFS(fmaps, m_maps_buf, 2048, true);

    if (m_maps_buf.size() == 0)
    {
        status_exit = PID_NOT_READ;
        throw std::runtime_error("Looks like proc/" + std::to_string(m_infos.pid) + "/maps is empty, I do a check in the past process");
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
    search_line(__mem, found, m_maps_buf);

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
bool Pmap::map_write(off_t __addr, void *__value, uint8_t __size)
{
    bool status_exit = false;

    Data data(__size);

    for (int i = 0; i < __size; i++)
        data.write('a');

    if (RemoteProcess::writeMem(__addr, &data) != WRITE_FAIL)
        status_exit = true;
    else        
        printf("Error\n");

    return status_exit;
}

/**
 *  @brief Reading mem for address in process to preference heap or stack
 *  @param __on address for init reading memory
 *  @param __off for end address mapped
 *  @return void
 */
bool Pmap::map_read(off_t __addr, uint8_t __type, Data &__data)
{
    bool status_exit = false;
    Data data(__type);

    // if (RemoteProcess::readMem(__addr, &data) != READ_FAIL)
    //{
    //     status_exit = true;
    //     __data = data;
    // }

    return status_exit;
}

/**
 * @brief
 *
 * @return int
 */
int Pmap::map_find(off_t __addr, uint64_t __length, std::string __find, uint8_t __type, std::vector<off_t> &__offsets)
{
    return RemoteProcess::findMem(__addr, __length, __type, __find, __offsets);
}

/**
 * @brief stop process if process stopped continue
 *
 */
void Pmap::map_stop(bool __enable)
{
     RemoteProcess::stopPid(__enable);
}

/**
 * @brief kill process
 *
 */
void Pmap::map_kill()
{
    if (m_infos.pid != 0)
        RemoteProcess::killPid();
}

/**
 * @brief get address on
 * @return off_t
 *
 * in address start for mem process
 */
off_t Pmap::get_addrOn() const
{
    return m_infos.addr_on;
}

/**
 * @brief get address off
 * @return off_t
 *
 * in address stop for mem process
 */
off_t Pmap::get_addrOff() const
{
    return m_infos.addr_off;
}

/**
 * @brief flags in address memory, r(read) w(write) ...
 *
 * @return std::string
 */
std::string Pmap::get_Flags()
{
    return m_infos.flags;
}

/**
 * @brief get address size mapped
 * @return size_t
 */
uint64_t Pmap::get_sizeAddress()
{
    return m_infos.addr_off - m_infos.addr_on;
}

/**
 * @brief take the pid utility as an example, the name of the pid,
 * just pass it as a NAME parameter, which will return the process name
 *
 * @param __utils parameter used to get the pid information
 * @return std::string
 */
std::string Pmap::get_utilsPid(uint8_t __utils)
{
    std::string buffer = " ";
    std::string pid_str = std::to_string(m_infos.pid);
    std::string name = PROC + pid_str;
    struct stat stats;

    if (m_infos.pid != 0)
    {
        switch (__utils)
        {
        case NAME:
            name += "/comm";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case WCHAN:
            name += "/wchan";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case SESSIONID:
            name += "/sessionid";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case CMDLINE:
            name += "/cmdline";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case LOGINUID:
            name += "/loginuid";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
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

        case EXEDIR:
            buffer = name += "/exe";
            break;

        case HOSTNAME:
            name = "/proc/sys/kernel/hostname";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case OSREALESE:
            name = "/proc/sys/kernel/osrelease";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case VERSION:
            name = "/proc/sys/kernel/version";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;

        case TYPE:
            name = "/proc/sys/kernel/ostype";
            m_FS.readFS(name, buffer, BUFFER_READ_UTILS);
            break;
        default:
            throw std::runtime_error("It was not possible to make the get at this value " + std::to_string(__utils));
        }
    }
    else
        throw std::runtime_error("Not read infos pid, set pid using map_pid(<pid>)");

    return buffer;
}