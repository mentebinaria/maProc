#ifndef ADDR_H
#define ADDR_H 

#define MAPS "/maps"
#define off_t long

struct __attribute__ ((packed)) Address_info
{
    off_t addr_on;
    off_t addr_off;
};

#endif // !ADDR_H