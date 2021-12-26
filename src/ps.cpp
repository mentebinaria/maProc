#include "include/ps.hpp"

#include <sstream>
#include <dirent.h>


/**
 * @brief Construct a new Ps:: Ps object
 * 
 */
Ps::Ps()
{ }

/**
 * @brief Destroy the Ps:: Ps object
 * 
 */
Ps::~Ps()
{ }

/**
 * @brief will get all processes from the magic folder / proc
 * will store in the parameters the name and pid equivalent to the process
 *
 * @param NameProcess pass an array as a parameter to store process names
 * @param PidProcess pass an array as a parameter to store process pids
 */
void Ps::Reading_DirProcess(std::vector<std::string> &NameProcess,
                            std::vector<std::string> &PidProcess)
{
    DIR *dir = opendir(PROC);
    struct dirent *dir_read;

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
                cmdline = "NULL";
            NameProcess.push_back(cmdline);
            FSCMDLINE.close();
            cmdline.clear();
        }
    }
    closedir(dir);
}