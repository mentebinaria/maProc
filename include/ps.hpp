#pragma once

#include <fstream>
#include <unordered_map>

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

    int Reading_DirProcess(std::unordered_map<std::string, std::string> &umap);
};