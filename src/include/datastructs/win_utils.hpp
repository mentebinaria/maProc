#pragma once

#define TITLE_WINDOW "maProc"
#define ICON_WINDOW "../src/assets/maProc.png"
#define ICON_PASS_PID "../src/assets/pid_proc.png"
#define ICON_RPASS_PID "../src/assets/rpid.png"
#define ICON_CLEAN "../src/assets/clean.png"
#define ICON_SEARCH "../src/assets/search.png"
#define ICON_CLOSE "../src/assets/close.png"
#define ICON_EDIT "../src/assets/edit.png"
#define ICON_NEW "../src/assets/new.png"
#define ICON_ABOUT "../src/assets/about.png"
#define ICON_HEX "../src/assets/hex.png"
#define ICON_KILL "../src/assets/kill.png"
#define ICON_STOP "../src/assets/stop.png"
#define ICON_PLAY "../src/assets/play.png"
#define ICON_HELP "../src/assets/help.png"
#define ICON_SCREEN "../src/assets/show.png"
#define ICON_SAVE "../src/assets/save.png"

#define NULL_STR "null"
#define CLEAN_ROW " "

/**
 * @brief heap and stack table enumeration
 */
enum Heap_Stack
{
    Address,
    Value,
    Memory
};

/**
 * @brief Infos table enumeration
 */
enum 
{
    Address_on,
    Address_off,
    Size_map,
    Flags
};

enum 
{
    Address_start,
    Address_stop,
    Perms,
    Offset,
    Pathname
};

enum 
{
    e_magic,
    e_version,
    e_entry,
    e_phoff,
    e_shoff,
    e_abiversion,
    e_flags,
    e_shentsize,
    e_shnum,
    e_shstrndx,
    e_class,
    e_abi,
    e_type,
    e_machine
};