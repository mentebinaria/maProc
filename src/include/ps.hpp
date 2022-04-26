#pragma once

#include <fstream>
#include <unordered_map>

#include "datastructs/erros.hpp"
#include "filedescriptor.hpp"

#define PROC "/proc/"
#define COMM "/comm"

class Ps : public FileDescriptor
{

public:
    Ps();
    virtual ~Ps();

    int Reading_DirProcess(std::unordered_map<std::string, std::string> &umap);
};
