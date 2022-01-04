#pragma once

#define MAPS "/maps"

struct __attribute__ ((packed)) Address_info
{
    off_t addr_on;
    off_t addr_off;
};