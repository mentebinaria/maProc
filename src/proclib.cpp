#include "include/proclib.hpp"


bool RemoteProcess::openProcess(pid_t __pid) 
{
    proc.pid = __pid;

    if (ptrace(PTRACE_ATTACH, proc.pid , NULL, NULL) < 0) {
        return false;
        // error is set in errno
    }

    // wait to the attach ends

    int wstatus;
    waitpid(proc.pid, &wstatus, 0);
    
    if (WIFSTOPPED(wstatus)) {
        // process has been attached and stopped, continue normally
        ptrace(PTRACE_CONT, proc.pid, NULL, NULL);
        return true; // success :)
    }

    return false;
}