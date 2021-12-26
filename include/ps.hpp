#ifndef PS_H
#define PS_H

#include <vector>
#include <fstream>

#define PROC    "/proc/"
#define CMDLINE "/cmdline"

class Ps
{
private:
    std::fstream FSCMDLINE;

public:
    Ps();
    ~Ps();
    
    void Reading_DirProcess(std::vector<std::string> &NameProcess,
                            std::vector<std::string> &PidProcess);
};

#endif // PS_H