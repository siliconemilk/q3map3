#ifndef MHASH_CRC32_H
#define MHASH_CRC32_H

#include "libdefs.h"

void mhash_clear_crc32(word32 * crc);
void mhash_get_crc32( const word32 * crc, void* ret);
void mhash_crc32(word32 * crc, const void *, int);
void mhash_crc32b(word32 * crc, const void *, int);

#endif
