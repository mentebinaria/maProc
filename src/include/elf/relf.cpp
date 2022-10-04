#include "relf.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <endian.h>
#include <string.h>
#include <byteswap.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
  #define ELFDATANATIVE ELFDATA2LSB
#elif __BYTE_ORDER == __BIG_ENDIAN
  #define ELFDATANATIVE ELFDATA2MSB
#else
  #error "Unknown machine endian"
#endif

// pointer for
const char *ElfPointer; //  pointer for  ElfMap
struct SELF Elf;

static void SwapEndian64 ( )
{
  Elf.elf64.Header->e_type = bswap_16 ( Elf.elf64.Header->e_type );
  Elf.elf64.Header->e_entry = bswap_64 ( Elf.elf64.Header->e_entry );
  Elf.elf64.Header->e_version = bswap_32 ( Elf.elf64.Header->e_version );
  Elf.elf64.Header->e_shstrndx = bswap_16 ( Elf.elf64.Header->e_shstrndx );
  Elf.elf64.Header->e_shnum = bswap_16 ( Elf.elf64.Header->e_shnum );
  Elf.elf64.Header->e_shoff = bswap_64 ( Elf.elf64.Header->e_shoff );
  Elf.elf64.Header->e_phentsize = bswap_16 ( Elf.elf64.Header->e_phentsize );
  Elf.elf64.Header->e_flags = bswap_16 ( Elf.elf64.Header->e_flags );
  Elf.elf64.Header->e_machine = bswap_16 ( Elf.elf64.Header->e_machine );
  Elf.elf64.Header->e_phoff = bswap_64 ( Elf.elf64.Header->e_phoff );
}

static void SwapEndian32()
{
  Elf.elf32.Header->e_type = bswap_16 ( Elf.elf32.Header->e_type );
  Elf.elf32.Header->e_entry = bswap_32 ( Elf.elf32.Header->e_entry );
  Elf.elf32.Header->e_version = bswap_32 ( Elf.elf32.Header->e_version );
  Elf.elf32.Header->e_shstrndx = bswap_16 ( Elf.elf32.Header->e_shstrndx );
  Elf.elf32.Header->e_shnum = bswap_16 ( Elf.elf32.Header->e_shnum );
  Elf.elf32.Header->e_shoff = bswap_32 ( Elf.elf32.Header->e_shoff );
  Elf.elf32.Header->e_phentsize = bswap_16 ( Elf.elf32.Header->e_phentsize );
  Elf.elf32.Header->e_flags = bswap_16 ( Elf.elf32.Header->e_flags );
  Elf.elf32.Header->e_machine = bswap_16 ( Elf.elf32.Header->e_machine );
  Elf.elf32.Header->e_phoff = bswap_32 ( Elf.elf32.Header->e_phoff );
}


/**
* @brief elf header
* @param [in] p_Elf.elf64.Header struct for usage elf header
* @return void
*/
void ElfStruct ( struct SELF &p_elf )
{
  if ( Elf.elf64.Header->e_ident[EI_CLASS] == ELFCLASS32 )
  {
    p_elf.elf32.Header = Elf.elf32.Header;
    p_elf.classElf = ELFCLASS32;

    p_elf.elf64.Header = NULL;
    p_elf.elf64.Program = NULL;
  }
  else
  {
    p_elf.elf64.Header = Elf.elf64.Header;
    p_elf.classElf = ELFCLASS64;

    p_elf.elf32.Header = NULL;
    p_elf.elf32.Program = NULL;
  }

  // verify endian
  if ( Elf.elf64.Header->e_ident[EI_DATA] != ELFDATANATIVE )
  {
    if ( Elf.elf64.Header->e_ident[EI_CLASS] == ELFCLASS64 )
      SwapEndian64();
    else
      SwapEndian32();
  }
}

/**
* @brief verify format elf for analisy
* @return bool if format elf true, return "true" else false
*/
static bool VElfMagic()
{
  return Elf.elf64.Header->e_ident[EI_MAG0] == 0x7f &&
         Elf.elf64.Header->e_ident[EI_MAG1] == 0x45 &&
         Elf.elf64.Header->e_ident[EI_MAG2] == 0x4c &&
         Elf.elf64.Header->e_ident[EI_MAG3] == 0x46;
}

/**
* @brief map binary in memory to analyze ELF
* @param [in] p_path path name bin ELF for mapper
* @return int if file is found and opened, return OPEN_SUCESS, else OPEN_FAIL
* @details it will just map elf binaries, that is, it will do a check when mapping
the file checking if the file in question is an ELF format
*/
int PElf ( const char *p_path )
{
  int status_exit = OPEN_SUCCESS;
  struct stat sb;

  int fd = open ( p_path, O_RDONLY );

  if ( fd != -1 )
  {
    fstat ( fd, &sb ); // get size file for mapping

    // mapper file ELF
    ElfPointer = ( char * ) mmap ( nullptr, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0 );

    if ( ElfPointer == MAP_FAILED )
    {
      perror ( ElfPointer );
      status_exit = ELF_INVALID;
    }
    else
    {
      Elf.elf64.Header = ( Elf64_Ehdr * ) ElfPointer;
      
      if ( Elf.elf64.Header->e_ident[EI_CLASS] == ELFCLASS32 )
        Elf.elf32.Header = ( Elf32_Ehdr * ) Elf.elf64.Header;
      if ( VElfMagic() == true ) // elf valid
        status_exit = ELF_VALID;
    }

    // close file
    close ( fd );
  }
  else
    perror ( p_path ), status_exit = OPEN_FAIL;


  return status_exit;
}


/**
* @brief get pointer e_ident elf
* @return char*
* @details limit EI_NIDENT
*/
unsigned char *ElfMagic()
{
  return Elf.elf64.Header->e_ident;
}