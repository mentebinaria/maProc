/**
* @file relf.h
* @author VitorMob
* @date 13 Mar 2022
* @copyright 2022 VitorMob
* @brief simple parser for analisys binary ELF for student
*/

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <byteswap.h>
#include <elf.h>

#include "datastructs/erros.hpp"
#include "datastructs/elf_structs.hpp"

//=================== functions usage for parser elf ===================
int PElf ( const char * );    // pass elf for analisys
void ElfStruct ( struct SELF& ); // get header information elf
unsigned char* ElfMagic(); // return pointer array magic elf
