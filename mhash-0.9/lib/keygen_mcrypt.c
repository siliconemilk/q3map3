/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
 *
 *    This library is free software; you can redistribute it and/or modify it 
 *    under the terms of the GNU Library General Public License as published 
 *    by the Free Software Foundation; either version 2 of the License, or 
 *    (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public
 *    License along with this library; if not, write to the
 *    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *    Boston, MA 02111-1307, USA.
 */



#include "mhash.h"
#include "libdefs.h"
#include "keygen.h"


/* Key generation using any hash algorithm */
int _mhash_gen_key_mcrypt(hashid algorithm, void *keyword, int key_size, void *salt, int salt_size,
		  unsigned char *password, int plen)
{
	word8* key=calloc(1, key_size);
	word8 *digest=NULL;
	int size = key_size;
	MHASH td;
	word8 *cp = key, *cp0 = key;
	int salt_z = 0;		/* flag to be used when freeing salt[] */
	int block_size = mhash_get_block_size(algorithm);

	if (salt == NULL) {
		salt_z = 1;
	}
	
	while (1) {
		td = mhash_init(algorithm);
		if (td==MHASH_FAILED) return -1;
		
		if (salt_z == 0)
			mhash(td, salt, salt_size);
		mhash(td, password, plen);
		if (cp - cp0 > 0)
			mhash(td, cp0, cp - cp0);
		digest=mhash_end(td);
		
		if (size > block_size) {
			memcpy(cp, digest, block_size);
			free(digest);
			size -= block_size;
			cp += block_size;
		} else {
			memcpy(cp, digest, size);
			free(digest);
			break;
		}
	}
	memcpy(keyword, key, key_size);
	free( key);
	return 0;
}


int _mhash_gen_key_pkdes( void *keyword, int key_size,
		 unsigned char *password, int plen)
{
	char* pkeyword=keyword;
	int cnt,i,c;

	if (plen>key_size) return -1;
	mhash_bzero( keyword, key_size);
	memcpy( keyword, password, plen);

	for (cnt = 0; cnt < key_size; cnt++) {
		c = 0;
		for (i = 0; i < 7; i++)
			if (pkeyword[cnt] & (1 << i))
				c++;
		if ((c & 1) == 0)
			pkeyword[cnt] |= 0x80;
		else
			pkeyword[cnt] &= ~0x80;
	}

	return 0;
}
