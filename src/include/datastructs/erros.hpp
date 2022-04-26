#pragma once

/**
 * @brief errors that can occur during the passing of the pid
 */
enum ERROR_PID
{
    PID_OVERDRIVE = -3,
    PID_NOT_READ,
    PID_NOT_FOUND,
    PID_SUCCESS
};

/**
 * @brief  erros files 
 */
enum ERROS_FILES
{
    OPEN_SUCCESS,
    OPEN_FAIL,
    READ_SUCCESS,
    READ_FAIL,
    WRITE_FAIL,
    WRITE_SUCCESS,
    EMPTY,
    FULL,
};

/**
 * @brief mem find 
 * 
 */
enum FIND_MEM
{
    NOT_FOUND,
    FOUND
};


enum STOPP
{
    STOPPED,
    CONT
};