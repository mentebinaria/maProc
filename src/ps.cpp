#include "include/ps.hpp"

#include <sstream>
#include <dirent.h>
#include <string.h>

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
 */
void Ps::Reading_DirProcess(std::unordered_map<std::string, infos> &p_umap)
{
    infos infos_pid;
    p_umap.clear();

    DIR *dir = opendir("/proc/");
    struct dirent *dir_read;

    if (dir == nullptr)
    {
        throw std::runtime_error("Error not open dir /proc");
    }

    while ((dir_read = readdir(dir)) != nullptr)
    {
        std::string file;
        if (strcmp(dir_read->d_name, "self") && strcmp(dir_read->d_name, "thread-self"))
        {
            try
            {
                file = "/proc/" + std::string(dir_read->d_name) + "/comm";
                readFS(file, infos_pid.name, 20);
                file = "/proc/" + std::string(dir_read->d_name) + "/loginuid"; 
                readFS(file, infos_pid.user, 20);
                infos_pid.pid =  dir_read->d_name;
                
                p_umap.insert(std::make_pair(infos_pid.name, infos_pid));
            }
            catch (std::exception &error)
            {
                file.clear();
            }
        }
    }

    closedir(dir);
}
