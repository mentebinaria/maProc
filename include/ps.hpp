#pragma once

#include <vector>
#include <fstream>

#include "include/datastructs/erros.hpp"

#define PROC "/proc/"
#define CMDLINE "/cmdline"

class Ps
{
private:
    std::fstream FSCMDLINE;

public:
    Ps();
    virtual ~Ps();

    int Reading_DirProcess(std::vector<std::string> &__NameProcess,
                           std::vector<std::string> &__PidProcess);
};