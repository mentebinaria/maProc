#ifndef STATUS_H
#define STATUS_H 

#include <string>

#define STATUS "/status"

#define off_t long

struct Status_info
{
    std::string Name;
    int Umask;
    std::string State;
    unsigned int Tgid;
    unsigned int Ngid;
    pid_t pid;
    unsigned int PPid;
    unsigned int TracerPid;
    unsigned int Uid;
    unsigned int Gid;
    uint16_t FDSize;
    std::string Groups;  
    int NStgid;
    int NSpid;
    int NSpgid;
    int NSsid;
    std::string VmPeak;
    std::string VmSize;
    std::string VmLck; 
    std::string VmPin; 
    std::string VmHWM; 
    std::string VmRSS; 
    std::string RssAnon;
    std::string RssFile;
    std::string RssShmem;
    std::string VmData;
    std::string VmStk; 
    std::string VmExe; 
    std::string VmLib; 
    std::string VmPTE; 
    std::string VmSwap;
    std::string HugetlbPages;
    int CoreDumping;
    int THP_enabled;
    unsigned int Threads;
    std::string SigQ;
    off_t SigPnd;
    off_t ShdPnd;
    off_t SigBlk;
    off_t SigIgn;
    off_t SigCgt;
    off_t CapInh;
    off_t CapPrm;
    off_t CapEff;
    off_t CapBnd;
    off_t CapAmb;
    int NoNewPrivs;
    int Seccomp;
    int Seccomp_filters;
    std::string Speculation_Store_Bypass;
    char Cpus_allowed;
    std::string Cpus_allowed_list;
    std::string Mems_allowed;
    int Mems_allowed_list;
    unsigned int voluntary_ctxt_switches;
    unsigned int nonvoluntary_ctxt_switches;
};

#endif // !STATUS_H 