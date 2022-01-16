#pragma once

#define TITLE_WINDOW "maProc v1.0"
#define ICON_WINDOW   "../src/assets/maProc.png"
#define ICON_PASS_PID "../src/assets/pid_proc.png"
#define ICON_CLEAN    "../src/assets/clean.png"
#define ICON_SEARCH   "../src/assets/search.png"
#define ICON_CLOSE    "../src/assets/close.png"
#define ICON_EDIT     "../src/assets/edit.png"
#define ICON_NEW      "../src/assets/new.png"

#define CLEAN_ROW     " "

/**
 * @brief heap and stack table enumeration
 */
enum Heap_Stack
{
    Address,
    Value
};

/**
 * @brief Infos table enumeration
 */
enum Infos
{
    Address_on,
    Address_off,
    Size_map
};
