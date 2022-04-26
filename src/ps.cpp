#include "include/ps.hpp"

#include <sstream>
#include <dirent.h>
#include <iostream>
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
 * @return OPEN_FAIL is /proc not opened else return OPEN_SUCCESS
 */
int Ps::Reading_DirProcess(std::unordered_map<std::string, std::string> &p_umap)
{
    p_umap.clear();

    int status_exit = OPEN_SUCCESS;
    DIR *dir = opendir(PROC);
    struct dirent *dir_read;

    if (dir == nullptr)
    {
        status_exit = OPEN_FAIL;
        throw std::runtime_error("Error not open dir /proc");
    }

    while ((dir_read = readdir(dir)) != nullptr)
    {
        std::string name;
        if (strcmp(dir_read->d_name, "self") && strcmp(dir_read->d_name, "thread-self"))
        {
            name = PROC + std::string(dir_read->d_name) + COMM;
            try
            {
                readFS(name, name, 20);
                (name.size() == 0) ? name = "N/S" : name;
                p_umap.insert(std::make_pair(name, dir_read->d_name));
            }
            catch (std::exception &error)
            {
                name.clear();
            }
        }
    }

    closedir(dir);
    return status_exit;
}
