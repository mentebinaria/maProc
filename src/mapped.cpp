#include <dirent.h>
#include <string>
#include <string.h>
#include <sys/ptrace.h>

#include "include/mapped.hpp"

//
//  code in manipulation and read memory
//  and write memory
//  class mapper_memory
//

//
//  function static for search to passing
//  std::string __find which string to search,
//  std::ofstream &__fs which file to upload
//  std::string &__load lading search file
//  return void
//
static void search(std::string __find,
                   std::fstream &__fs, std::string &__load)
{
    std::string str;
    std::size_t find_pos = 0;

    while (getline(__fs, str))
    {
        find_pos = str.find(__find);
        if (find_pos != std::string::npos)
        {
            for (std::size_t i = find_pos; str[i] != '\0'; i--)
                __load += str[i];

            __load += '\n';
        }
    }

    for (size_t i = __load.size(); i > 0; i--)
        str += __load[i];

    __load = str;
    str.clear();
}

//
//  mem write passing address for to modify
//  off_t __addr pass address for modify
//
bool mapper_memory::mem_write(off_t __addr, void *__val)
{
    bool status_exit = true;

    return status_exit;
}

//
//  reading mem for address in process to preference heap or stack
//  off_t __on address for init reading memory
//  off_t __off for end address mapped
//  return bool
//
void mapper_memory::mem_read(std::string __pid, std::string __on, std::string __off)
{
    ptrace(PTRACE_ATTACH, std::stoi(__pid), 0, 0); // ptrace attach for reading process

    off_t on_long = std::stoul(__on, nullptr, 16);
    off_t off_long = std::stoul(__off, nullptr, 16);

    if (on_long == 0)
        return;

    for (off_t i = on_long; i <= off_long; i++)
    {
        off_t data = ptrace(PTRACE_PEEKTEXT, std::stoi(__pid), i, 0);
    }
}

//
//  constructor
//
mapper_memory::mapper_memory()
{
}

//
//  destructor
//
mapper_memory::~mapper_memory()
{
}

//
// check the process pid if it exists
// std::string __pid pass pid for check is valid process
// return bool
//
bool mapper_memory::verify_pid(std::string __pid)
{
    pid = __pid;
    std::string proc = PROC + __pid;

    DIR *dir = opendir(proc.c_str());

    closedir(dir);
    return (dir) ? true : false;
}

//
// verify mem if process usage
// std::string __mem passer mem for usage process 
// return bool
//
bool mapper_memory::verify_mem(std::string __mem)
{
    bool status_exit = true;

    std::string maps = PROC + pid + MAPS;
    std::fstream fs;

    fs.open(maps);
    if (!fs.is_open())
        status_exit = false;

    std::string found;
    search(__mem, fs, found);

    if (found.size() == 0)
        status_exit = false;
    else
    {
        mem = found;
        mem_address();
    }
    return status_exit;
}

//
//  reading mem for address in process to preference heap or stack
//  __mem what memory for map process (stack or heap?)
//  return bool
//
void mapper_memory::mem_address()
{
    addr_on.clear();
    addr_off.clear();

    // get address start
    for (std::size_t i = 1; i <= mem.size(); i++)
    {
        if (mem[i] == '-')
            break;
        addr_on += mem[i];
    }

    // get address end
    for (std::size_t i = addr_on.size() + 2; i <= mem.size(); i++)
    {
        addr_off += mem[i];
        if (mem[i] == ' ')
            break;
    }

    if (strlen(addr_on.data()) == 0)
        std::exit(EXIT_FAILURE);

    mem.clear();
}

//
// get addr off
// return std::string
//
std::string mapper_memory::get_addr_on()
{
    return addr_on;
}

//
// get address on
// return std::string
//
std::string mapper_memory::get_addr_off()
{
    return addr_off;
}

//
// get address size mapped
// return size_t
//

size_t mapper_memory::get_size_address()
{
    off_t address_size = std::stoul(addr_off, nullptr, 16) - std::stoul(addr_on, nullptr, 16);
    return address_size;
}