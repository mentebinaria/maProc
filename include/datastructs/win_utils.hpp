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
enum Infos
{
    Address_on,
    Address_off,
    Size_map,
    Flags
};
