#pragma once

#include <vector>
#include <fstream>

#include "include/datastructs/erros.hpp"

#define PROC "/proc/"
#define COMM "/comm"

class Ps
{
private:
    std::fstream FS;

public:
    Ps();
    virtual ~Ps();

    int Reading_DirProcess(std::vector<std::string> &__NameProcess,
                           std::vector<std::string> &__PidProcess);
};