#include <dirent.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>

#include "include/pmap.hpp"
#include "include/datastructs/erros.hpp"


/**
 * @brief function verify pid for passed in user
 * @param p_pid pass pid for verification is exist
 * @param p_max pass to max pid for verification for not ultrapassing pid limit
 * @return int
 *
 * if process exist for linux return true else false
 */
static int verify_pid ( pid_t p_pid, pid_t p_max )
{
  int status_exit = PID_SUCCESS;

  if ( p_pid <= 0 || p_pid > p_max )
  {
    status_exit = PID_OVERDRIVE;
    throw std::runtime_error ( "Pid " + std::to_string ( p_pid ) + " passed is not valid, valid pid not less 0 or larger " + std::to_string ( p_max ) );
  }
  else
  {
    std::string proc = PROC + std::to_string ( p_pid );
    DIR *dir = opendir ( proc.c_str() );

    if ( dir == nullptr )
    {
      status_exit = PID_NOT_FOUND;
      throw std::runtime_error ( "Pid not found, verify to pid and pass pid valid" );
    }
    else
      closedir ( dir );
  }

  return status_exit;
}

/**
 *  @brief Function static for search will get the line completely by
 *  traversing the string backwards, will clear the string to be loaded the line
 *  @param p_find which string to search
 *  @param &p_load lading search buffer
 *  @param &p_buffer string to searching
 *  @return void
 */
static void search_line ( std::string p_find, std::string &p_load, std::string &p_buffer )
{
  std::size_t find_pos = p_buffer.find ( p_find );

  if ( find_pos != std::string::npos )
  {
    for ( ; p_buffer[find_pos] != '\n'; find_pos-- )
      p_load += p_buffer[find_pos];
  }

  std::string format;

  for ( std::size_t i = p_load.size(); i > 0; i-- )
    format += p_load[i];

  p_load = format;

  CLEAR_STRING ( format );
}

/**
 * @brief will do a get the addresses from the line
 * @param __line which line to pass to get the addresses

 * @return void
 */
void Pmap::split_mem_address ( std::string &p_foo )
{
  // get address start
  std::string addr_on;

  for ( std::size_t i = 1; i <= p_foo.size(); i++ )
  {
    if ( p_foo[i] == '-' )
      break;

    addr_on += p_foo[i];
  }

  // get address end
  std::string addr_off;
  std::string flags;

  for ( std::size_t i = addr_on.size() + 2; i <= p_foo.size(); i++ )
  {
    addr_off += p_foo[i];

    if ( p_foo[i] == ' ' )
    {
      for ( std::size_t i = addr_on.size() + addr_off.size() + 2; i < p_foo.size(); i++ )
      {
        flags += p_foo[i];

        if ( p_foo[i] == ' ' )
          break;
      }

      break;
    }
  }

  if ( addr_on.size() == 0 || addr_off.size() == 0 )
  {
    throw std::runtime_error ( "Error not split address_on and address_off" );
    return;
  }

  // convert string to off_t
  off_t addr_off_long = ( off_t ) std::stoul ( addr_off, nullptr, 16 );
  off_t addr_on_long = ( off_t ) std::stoul ( addr_on, nullptr, 16 );

  m_infos.addr_on = ( off_t ) addr_on_long, m_infos.addr_off = ( off_t ) addr_off_long, m_infos.flags = flags;

  CLEAR_STRING ( p_foo );
  CLEAR_STRING ( flags );
}

/**
 *  @brief Constructor
 */
Pmap::Pmap()
{
  m_infos.pid_max = 0;
  m_infos.pid = 0;
  m_infos.addr_on = 0;
  m_infos.addr_off = 0;
  PID_MAX
}

/**
 *  @brief Destructor
 */
Pmap::~Pmap()
{
  CLEAR_STRING ( m_maps_buf );
}

/**
 *  @brief  Mapper process passing pid
 *  @param  p_pid pid of the process to be mapped
 *  @return int
 *
 *  pid not found return PID_NOT_FOUND
 *  if the pid exists ira return PID_SUCCESS
 */
int Pmap::map_pid ( pid_t p_pid )
{
  m_infos.pid = p_pid;

  int status_exit = verify_pid ( m_infos.pid, m_infos.pid_max );
  RemoteProcess::openProcess ( m_infos.pid );

  std::string pid_str = std::to_string ( m_infos.pid );
  std::string fmaps = PROC + pid_str + MAPS;

  m_FS.readFS ( fmaps, m_maps_buf, 2048, true );

  if ( m_maps_buf.size() == 0 )
  {
    status_exit = PID_NOT_READ;
    throw std::runtime_error ( "Looks like proc/" + std::to_string ( m_infos.pid ) + "/maps is empty, I do a check in the past process" );
  }

  return status_exit;
}

/**
 * @brief verify mem if process usage
 * @param p_mem memory to be mapped will check if it exists and map such memory
 * @return bool
 *
 * if the use of memory is `not` being used it returns true or returns false
 * if the memory is not being used by the process, it will map right away
 * that the memory is being used
 */
bool Pmap::map_mem ( std::string p_mem )
{
  bool status_exit = true;

  std::string found;
  search_line ( p_mem, found, m_maps_buf );

  if ( found.size() == 0 )
  {
    status_exit = false;
    throw std::runtime_error ( "Process not using memory " + p_mem );
  }
  else
    split_mem_address ( found );

  return status_exit;
}

/**
 *  @brief Mem write passing address for to modify
 *  @param p_addr pass address for modify
 *  @param __val pass value for modify
 *  @return bool
 */
bool Pmap::map_write ( off_t p_addr, void *__value, uint p_size )
{
  bool status_exit = false;

  Data data ( ( uint8_t * ) __value, p_size );

  if ( RemoteProcess::writeMem ( p_addr, &data ) != WRITE_FAIL )
    status_exit = true;

  return status_exit;
}

/**
 *  @brief Reading mem for address in process to preference heap or stack
 *  @param __on address for init reading memory
 *  @param __off for end address mapped
 *  @return bool
 */
bool Pmap::map_read ( off_t p_addr, uint p_size, Data &p_data )
{
  bool status_exit = false;

  if ( RemoteProcess::readMem ( p_addr, p_addr + p_size, &p_data ) != READ_FAIL )
    status_exit = true;

  return status_exit;
}

/**
 * @brief will scan the memory looking for the
 * desired values ​​and the specific type
 *
 * @param p_addr  start address for find
 * @param p_length size memory for read and find
 * @param p_find value for find
 * @param p_type type for search
 * @param p_offsets store the found offsets in a vector
 * @return int
 */
int Pmap::map_find ( off_t p_addr, uint64_t p_length, std::string p_find, uint8_t p_type, std::vector<off_t> &p_offsets )
{
  return RemoteProcess::findMem ( p_addr, p_length, p_type, p_find, p_offsets );
}

/**
 * @brief stop process if process stopped continue
 *
 */
void Pmap::map_stop ( bool p_enable )
{
  RemoteProcess::stopPid ( p_enable );
}

/**
 * @brief kill process
 *
 */
void Pmap::map_kill()
{
  if ( m_infos.pid != 0 )
    RemoteProcess::killPid();
}

/**
 * @brief get address on
 * @return off_t
 *
 * in address start for mem process
 */
off_t Pmap::get_addrOn() const
{
  return m_infos.addr_on;
}

/**
 * @brief get address off
 * @return off_t
 *
 * in address stop for mem process
 */
off_t Pmap::get_addrOff() const
{
  return m_infos.addr_off;
}

/**
 * @brief flags in address memory, r(read) w(write) ...
 *
 * @return std::string
 */
std::string Pmap::get_Flags()
{
  return m_infos.flags;
}

/**
 * @brief get address size mapped
 * @return size_t
 */
uint64_t Pmap::get_sizeAddress()
{
  return m_infos.addr_off - m_infos.addr_on;
}

/**
 * @brief take the pid utility as an example, the name of the pid,
 * just pass it as a NAME parameter, which will return the process name
 *
 * @param p_utils parameter used to get the pid information
 * @return std::string
 */
std::string Pmap::get_utilsPid ( uint8_t p_utils )
{
  std::string buffer;
  struct stat stats;
  std::string pid_str = std::to_string ( m_infos.pid );
  std::string name = PROC + pid_str;

  if ( m_infos.pid != 0 )
  {
    switch ( p_utils )
    {
      case NAME:
        name += "/comm";
        m_FS.readFS ( name, buffer, BUFFER_READ_UTILS );
        break;

      case WCHAN:
        name += "/wchan";
        m_FS.readFS ( name, buffer, BUFFER_READ_UTILS );
        break;

      case SESSIONID:
        name += "/sessionid";
        m_FS.readFS ( name, buffer, BUFFER_READ_UTILS );
        break;

      case CMDLINE:
        name += "/cmdline";
        m_FS.readFS ( name, buffer, BUFFER_READ_UTILS );
        break;

      case LOGINUID:
        name += "/loginuid";
        m_FS.readFS ( name, buffer, BUFFER_READ_UTILS );
        break;

      case SIZEBIN:
        name += "/exe";

        if ( stat ( name.data(), &stats ) == 0 )
          buffer = std::to_string ( stats.st_size );

        break;

      case BLOCKSIZEBIN:
        name += "/exe";

        if ( stat ( name.data(), &stats ) == 0 )
          buffer = std::to_string ( stats.st_blksize );

        break;

      case EXEDIR:
        buffer = name += "/exe";
        break;

      default:
        throw std::runtime_error ( "It was not possible to make the get at this value " + std::to_string ( p_utils ) );

    }
  }

  if ( buffer.size() == 0 )
  {
    switch ( p_utils )
    {
      case HOSTNAME:
        m_FS.readFS (  "/proc/sys/kernel/hostname", buffer, BUFFER_READ_UTILS );
        break;

      case OSREALESE:
        m_FS.readFS ( "/proc/sys/kernel/osrelease", buffer, BUFFER_READ_UTILS );
        break;

      case VERSION:
        m_FS.readFS ( "/proc/sys/kernel/version", buffer, BUFFER_READ_UTILS );
        break;

      case TYPE:
        m_FS.readFS ( "/proc/sys/kernel/ostype", buffer, BUFFER_READ_UTILS );
        break;

      default:
        throw std::runtime_error ( "It was not possible to make the get at this value " + std::to_string ( p_utils ) );
    }
  }

  return buffer;
}
