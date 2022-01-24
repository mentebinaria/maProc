#pragma once

#define MAPS "/maps"

/**
 * @brief struct address start and end
 */
struct Address_info
{
    off_t addr_on;
    off_t addr_off;

} __attribute__((packed));