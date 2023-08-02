#pragma once

#define TITLE_WINDOW "maProc"
#define ICON_WINDOW "../src/gui/assets/maProc.png"
#define ICON_PASS_PID "../src/gui/assets/pid_proc.png"
#define ICON_RPASS_PID "../src/gui/assets/rpid.png"
#define ICON_CLEAN "../src/gui/assets/clean.png"
#define ICON_SEARCH "../src/gui/assets/search.png"
#define ICON_CLOSE "../src/gui/assets/close.png"
#define ICON_EDIT "../src/gui/assets/edit.png"
#define ICON_NEW "../src/gui/assets/new.png"
#define ICON_ABOUT "../src/gui/assets/about.png"
#define ICON_HEX "../src/gui/assets/hex.png"
#define ICON_KILL "../src/gui/assets/kill.png"
#define ICON_STOP "../src/gui/assets/stop.png"
#define ICON_PLAY "../src/gui/assets/play.png"
#define ICON_HELP "../src/gui/assets/help.png"
#define ICON_SCREEN "../src/gui/assets/show.png"
#define ICON_SAVE "../src/gui/assets/save.png"

#define NULL_STR "(null)"
#define CLEAN_ROW ""

/**
 * @brief heap and stack table enumeration
 */
enum
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


/**
 * @brief pid table enumeration
 */
enum
{
    Pid,
    Name,
    User
};
