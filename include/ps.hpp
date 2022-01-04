#pragma once

#include <vector>
#include <fstream>

#include "include/datastructs/erros.hpp"

#define PROC "/proc/"
#define COMM "/comm"

class Ps
{
private:
    std::fstream FSCMDLINE;

public:
    Ps();
    virtual ~Ps();

    std::string Get_NamePid(pid_t __pid);
    int Reading_DirProcess(std::vector<std::string> &__NameProcess,
                           std::vector<std::string> &__PidProcess);
};