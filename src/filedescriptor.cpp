#include "../include/filedescriptor.hpp"
#include "../include/datastructs/utils.hpp"
#include "../include/datastructs/erros.hpp"

#include <sstream>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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
 * @return long, if reading file success return READ_SUCCESS else if not len READ_FAIL not open file return OPEN_FAIL
 */
long FileDescriptor::readFS(std::string __name, std::string &__buffer, long __nblock = 256, bool __blockn2)
{
    CLEAR_STRING(__buffer)

    long status_exit = READ_SUCCESS;

    off_t nblock = __nblock;
    const char *name = __name.data();

    __buffer.reserve(__nblock);
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

        if (__buffer.size() == 0)
        {
            status_exit = READ_FAIL;
            throw std::runtime_error("Not possible read file, check permission " + __name);
        }

        close(FS);
    }
    else
    {
        status_exit = OPEN_FAIL;
        throw std::runtime_error("Error open file " + __name);
    }

    return status_exit;
}

/**
 * @brief analysis buffer file in stack
 *
 * @param __name name file
 * @param __nblock block size
 * @param Analysis func callback for analysis buffer
 * @return int
 */
int FileDescriptor::readSeachFS(std::string __name, long __nblock, std::string __find,
                                int(Analysis)(char *__buffer, std::string __find), off_t __offset)
{
    long status_exit = OPEN_SUCCESS;

    const char *name = __name.data();
    char buffer[__nblock];

    int FS = open(name, O_RDONLY);
    if (FS > 0)
    {
        do
        {
            memset(buffer, 0, sizeof(buffer));

            if (pread(FS, buffer, sizeof(buffer), __offset) == -1)
            {
                status_exit = READ_FAIL;
                break;
            }

        } while (Analysis(buffer, __find) != FOUND);

        close(FS);
    }
    else
    {
        status_exit = OPEN_FAIL;
        throw std::runtime_error("Error open file " + __name);
    }

    return status_exit;
}