#pragma once

#include <fstream>
#include <unordered_map>

#include "src/include/data/erros.hpp"
#include "src/include/fd/filedescriptor.hpp"

struct infos
{
  std::string pid;
  std::string name;
  std::string user;

};

class Ps : public FileDescriptor
{

 public:
  Ps();
  virtual ~Ps();

  void Reading_DirProcess ( std::unordered_map<std::string, infos> &umap );
};
