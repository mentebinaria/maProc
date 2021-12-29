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

/**
 * @brief will get all processes from the magic folder / proc
 * will store in the parameters the name and pid equivalent to the process
 *
 * @param NameProcess pass an array as a parameter to store process names
 * @param PidProcess pass an array as a parameter to store process pids
 *
 * @return OPEN_FAIL is /proc not opened else return OPEN_SUCCESS
 */
int Ps::Reading_DirProcess(std::vector<std::string> &NameProcess,
                           std::vector<std::string> &PidProcess)
{
    int status_exit = OPEN_SUCCESS;
    DIR *dir = opendir(PROC);
    struct dirent *dir_read;

    if (dir == NULL)
    {
        throw std::runtime_error("Error not open dir /proc");
        status_exit = OPEN_FAIL;
    }

    while ((dir_read = readdir(dir)) != NULL)
    {
        std::string verify = PROC + std::string(dir_read->d_name) + CMDLINE;

        FSCMDLINE.open(verify);
        if (FSCMDLINE.is_open())
        {
            std::string cmdline;
            PidProcess.push_back(dir_read->d_name);
            getline(FSCMDLINE, cmdline);
            if (cmdline.size() == 0)
                cmdline = "name not found";
            NameProcess.push_back(cmdline);
            FSCMDLINE.close();
            cmdline.clear();
        }
    }

    closedir(dir);
    return status_exit;
}