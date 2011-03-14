#ifndef MHASH_ADLER32_H
#define MHASH_ADLER32_H

#include "libdefs.h"

void mhash_clear_adler32(word32 * adler);
void mhash_get_adler32( const word32 * adler, void* ret);
void mhash_adler32(word32 * adler, const void *, int);

#endif
