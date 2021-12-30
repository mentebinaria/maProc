#include <dirent.h>
#include <sys/ptrace.h>
#include <sstream>

#include "include/pmap.hpp"
#include "include/datastructs/utils.hpp"
#include "include/datastructs/erros.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

/**
 * @brief Construct a new File Descriptor:: File Descriptor object
 *
 */
FileDescriptor::FileDescriptor()
{
}

/**
 * @brief Destroy the File Descriptor:: File Descriptor object
 *
 */
FileDescriptor::~FileDescriptor()
{
}

/**
 * @brief
 *
 * @param __name
 * @param __buffer
 */
void FileDescriptor::SaveBuffer(std::string __name, std::string &__buffer)
{
    std::stringstream str;

    CLEAR_STRING(__buffer)
    CLEAR_STRING(str);

    const char *name = __name.data();
    int FS = open(name, O_RDONLY);
    if (FS < 0)
        throw std::runtime_error("Error open file" + __name);

    char buffer[1024];

    if (FS != -1)
    {
        do
        {
            memset(buffer, 0, sizeof(buffer));
            if (read(FS, buffer, sizeof(buffer)) == 0)
                break;

            str << buffer;

        } while (FS != EOF);
    }

    __buffer = str.str();

    close(FS);
}

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

    std::string proc = PROC + std::to_string(__pid);
    DIR *dir = opendir(proc.c_str());

    if (dir == NULL)
    {
        status_exit = PID_NOT_FOUND;
        throw std::runtime_error("Pid not found, verify to pid and pass pid valid");
    }
    closedir(dir);

    return status_exit;
}

/**
 *  @brief Function static for search to passing
 *  @param __find which string to search
 *  @param &__load lading search buffer
 *  @param &__buffer string to searching
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
void Pmap::split_mem_address(std::string __line)
{
    std::string addr_on;
    std::string addr_off;

    if (addr_on.size() != 0 && addr_off.size() != 0)
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

    // convert to string for off_t
    off_t addr_on_long = std::stoul(addr_on, nullptr, 16);
    off_t addr_off_long = std::stoul(addr_off, nullptr, 16);

    if (addr_on.size() == 0 || addr_on.size() == 0)
        std::runtime_error("Error not split address_on and address_off");
    else
    {
        ADDR_INFO.addr_on = addr_on_long;
        ADDR_INFO.addr_off = addr_off_long;
    }

    CLEAR_STRING(__line)
}

/**
 * @brief get status process
 */
void Pmap::split_status_process()
{
}

/**
 *  @brief Constructor
 */
Pmap::Pmap() throw(){
    PID_MAX}

/**
 *  @brief Destructor
 */
Pmap::~Pmap()
{
}

/**
 *  @brief  Mapper process passing pid
 *  @param  __pid pid of the process to be mapped
 *  @return int
 *
 *  pid not found return PID_NOT_FOUND
 *  if the pid exists ira return PID_SUCCESS
 *
 */
int Pmap::map_pid(pid_t __pid)
{
    pid = __pid;

    int status_exit = verify_pid(pid, pid_max);
    RemoteProcess::openProcess(pid);

    std::string pid_str = std::to_string(pid);
    std::string maps = PROC + pid_str + MAPS;
    std::string status = PROC + pid_str + STATUS;

    FS.SaveBuffer(status, status_buf);
    FS.SaveBuffer(maps, maps_buf);

    if (maps_buf.size() == 0 || status_buf.size() == 0)
    {
        status_exit = PID_NOT_READ;
        throw std::runtime_error("Not possible len infos to process");
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
bool Pmap::map_write()
{
    return true;
}

/**
 *  @brief Reading mem for address in process to preference heap or stack
 *  @param __on address for init reading memory
 *  @param __off for end address mapped
 *  @return void
 */
bool Pmap::map_read()
{
    return true;
}

/**
 * @brief get address on
 * @return off_t
 *
 * in address start for mem process
 */
off_t Pmap::get_addrOn() const
{
    return ADDR_INFO.addr_on;
}

/**
 * @brief get address off
 * @return off_t
 *
 * in address stop for mem process
 */
off_t Pmap::get_addrOff() const
{
    return ADDR_INFO.addr_off;
}

/**
 * @brief get address size mapped
 * @return size_t
 */
size_t Pmap::get_sizeAddress()
{
    return ADDR_INFO.addr_off - ADDR_INFO.addr_on;
}