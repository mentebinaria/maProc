#include "include/filedescriptor.hpp"
#include "include/datastructs/utils.hpp"
#include "include/datastructs/erros.hpp"

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
 * @param p_name name for read file
 * @param p_buffer in which variable will you store
 * @param p_nblock block size for reading
 * @param p_blockm if this option is turned on the block size will multiply by 2 per p_nblock
 *
 * @return long, if reading file success return READ_SUCCESS else if not len READ_FAIL not open file return OPEN_FAIL
 */
long FileDescriptor::readFS ( std::string p_name, std::string &p_buffer, long p_nblock = 256, bool p_blockm )
{

  CLEAR_STRING ( p_buffer )

  long status_exit = READ_SUCCESS;

  off_t nblock = p_nblock;
  const char *name = p_name.data();

  p_buffer.reserve ( p_nblock );
  int FS = open ( name, O_RDONLY );

  if ( FS > 0 )
  {
    do
    {
      char buffer[nblock];
      memset ( buffer, 0, nblock ); // clean buffer array

      if ( read ( FS, buffer, nblock ) == 0 )
        break;

      p_buffer += buffer; // save block in variable p_buffer

      // increase the bytes of the block thus decreasing the read cycles
      // it could possibly end up exceeding the file buffer limit by allocating more than necessary
      ( p_blockm ) ? nblock += p_nblock : nblock;

    } while ( FS != EOF );

    if ( p_buffer.size() == 0 )
    {
      status_exit = READ_FAIL;
      throw std::runtime_error ( "Not possible read file, check permission " + p_name );
    }

    close ( FS );
  }
  else
  {
    status_exit = OPEN_FAIL;
    throw std::runtime_error ( "Error open file " + p_name );
  }

  return status_exit;
}
