#include "include/ps.hpp"

#include <sstream>
#include <dirent.h>

/**
 * @brief Construct a new Ps:: Ps object
 *
 */
Ps::Ps()
{
}

/**
 * @brief Destroy the Ps:: Ps object
 *
 */
Ps::~Ps()
{
}

std::string Ps::Get_NamePid(pid_t __pid)
{
    DIR *dir = opendir(PROC);
    struct dirent *dir_read;

    if (dir == NULL)
        throw std::runtime_error("Error not open dir /proc");

    dir_read = readdir(dir);
    std::string comm = PROC + std::string(dir_read->d_name) + COMM;

    FSCMDLINE.open(comm);
    if (FSCMDLINE.is_open())
    {
        getline(FSCMDLINE, comm);
        (comm.size() == 0) ? comm = "not found" : comm;

        FSCMDLINE.close();
        closedir(dir);
    }

    return comm; 
}

/**
 * @brief will get all processes from the magic folder / proc
 * will store in the parameters the name and pid equivalent to the process
 *
 * @param NameProcess pass an array as a parameter to store process names
 * @param PidProcess pass an array as a parameter to store process pids
 *
 * @return OPEN_FAIL is /proc not opened else return OPEN_SUCCESS
 */
int Ps::Reading_DirProcess(std::vector<std::string> &__NameProcess,
                           std::vector<std::string> &__PidProcess)
{
    __NameProcess.clear();
    __PidProcess.clear();

    int status_exit = OPEN_SUCCESS;
    DIR *dir = opendir(PROC);
    struct dirent *dir_read;

    if (dir == NULL)
    {
        status_exit = OPEN_FAIL;
        throw std::runtime_error("Error not open dir /proc");
    }

    while ((dir_read = readdir(dir)) != NULL)
    {
        std::string cmdline = PROC + std::string(dir_read->d_name) + COMM;

        FSCMDLINE.open(cmdline);
        if (FSCMDLINE.is_open())
        {
            getline(FSCMDLINE, cmdline);
            (cmdline.size() == 0) ? cmdline = "not found" : cmdline;

            __PidProcess.push_back(dir_read->d_name);
            __NameProcess.push_back(cmdline);

            FSCMDLINE.close();
        }
        else
            continue;
    }

    closedir(dir);
    return status_exit;
}