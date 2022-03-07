#pragma once

/**
 * this file descriptor was created in order to read 'peseudos files'
 * from linux as we are parsing /proc/, we created this filedescriptor
 * to make it easy to manipulate linux files
 */

#include <string>

/**
 * @brief class for len pseudos files /proc
 *
 */
class FileDescriptor
{
public:
    FileDescriptor();
    ~FileDescriptor();

    long readFS(std::string , std::string &, long , bool = false);
};