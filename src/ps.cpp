#include "include/ps.hpp"

#include <sstream>
#include <dirent.h>
#include <iostream>

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
int Ps::Reading_DirProcess(std::unordered_map<std::string,std::string> &umap)
{
    umap.clear();

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
        std::string str = PROC + std::string(dir_read->d_name) + COMM;

        FS.open(str);
        if (FS.is_open())
        {
            getline(FS, str);
            (str.size() == 0) ? str = "NF" : str;
            
            umap.insert(std::make_pair(str, dir_read->d_name));

            FS.close();
        }
    }

    closedir(dir);
    return status_exit;
}