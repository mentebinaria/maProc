#pragma once

/**
 * @brief errors that can occur during the passing of the pid
 */
enum ERROR_PID
{
    PID_OVERDRIVE = -3,
    PID_NOT_READ,
    PID_NOT_FOUND
};

/**
 * @brief 
 */
enum ERROS_SUCESS_MEM
{
    OPEN_SUCCESS,
    OPEN_FAIL,
    READ_SUCCESS,
    READ_FAIL,
    WRITE_FAIL,
    WRITE_SUCCESS
};