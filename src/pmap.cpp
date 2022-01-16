#include <dirent.h>
#include <sys/ptrace.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "include/pmap.hpp"
#include "include/datastructs/utils.hpp"
#include "include/datastructs/erros.hpp"

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
 * @brief this method will read the entire past file
 *
 * @param __name name for read file
 * @param __buffer in which variable will you store
 * @param __nblock block size for reading
 * @param __blockn2 if this option is turned on the block size will multiply by 2 per __nblock
 *
 * @return if reading file success return READ_SUCCESS else if not open file return OPEN_FAIL
 */
int FileDescriptor::readFS(std::string __name, std::string &__buffer,
                           off_t __nblock = 256, bool __blockn2 = true)
{
    CLEAR_STRING(__buffer)

    int status_exit = READ_SUCCESS;

    off_t nblock = __nblock;
    const char *name = __name.data();

    int FS = open(name, O_RDONLY);
    if (FS > 0)
    {
        do
        {
            char buffer[nblock];
            memset(buffer, 0, sizeof(buffer)); // clean buffer array

            if (read(FS, buffer, sizeof(buffer)) == 0)
                break;

            __buffer += buffer; // save block in variable __buffer

            // increase the bytes of the block thus decreasing the read cycles
            // it could possibly end up exceeding the file buffer limit by allocating more than necessary
            (__blockn2) ? nblock += __nblock : nblock;

        } while (FS != EOF);
        
        close(FS);
    }
    else
    {
        status_exit = OPEN_FAIL;
        throw std::runtime_error("Error open file" + __name);
    }

    return status_exit;
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

    if (dir == nullptr)
    {
        status_exit = PID_NOT_FOUND;
        throw std::runtime_error("Pid not found, verify to pid and pass pid valid");
    }
    else
        closedir(dir);

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

    // convert to string for off_t
    unsigned int addr_off_long = std::stoul(addr_off, nullptr, 16);
    unsigned int addr_on_long = std::stoul(addr_on, nullptr, 16);

    if (addr_on.size() == 0 || addr_on.size() == 0)
        std::runtime_error("Error not split address_on and address_off");
    else
    {
        ADDR_INFO.addr_on = addr_on_long;
        ADDR_INFO.addr_off = addr_off_long;
    }

    CLEAR_STRING(__foo)
}

/**
 * @brief get status process
 */
void Pmap::split_status_process(std::string __foo)
{
    
}

/**
 *  @brief Constructor
 */
Pmap::Pmap()
{
    PID_MAX
}

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
 */
int Pmap::map_pid(pid_t __pid)
{
    pid = __pid;

    int status_exit = verify_pid(pid, pid_max);
    RemoteProcess::openProcess(pid);

    std::string pid_str = std::to_string(pid);
    std::string fmaps = PROC + pid_str + MAPS;
    std::string fstatus = PROC + pid_str + STATUS;

    FS.readFS(fstatus, status_buf, 1024);
    FS.readFS(fmaps, maps_buf, 1024);

    if (maps_buf.size() == 0)
    {
        status_exit = PID_NOT_READ;
        throw std::runtime_error("Looks like proc/" + std::to_string(pid) + "/maps is empty, I do a check in the past process");
    }

    split_status_process(status_buf);

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
    // Data data(10);
    // RemoteProcess::readMem(ADDR_INFO.addr_on, &data);
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
off_t Pmap::get_sizeAddress()
{
    return ADDR_INFO.addr_off - ADDR_INFO.addr_on;
}