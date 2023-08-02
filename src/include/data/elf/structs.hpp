#pragma once

#include <elf.h>

struct SELF
{
  struct elf64_s
  {
    Elf64_Ehdr *Header;
    Elf64_Phdr *Program;

  } elf64;

  struct elf32_s
  {
    Elf32_Ehdr *Header;
    Elf32_Phdr *Program;

  } elf32;

  int classElf;
};