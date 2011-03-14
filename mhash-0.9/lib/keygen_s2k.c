/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
 *    Copyright (C) 2002 Nikos Mavroyanopoulos
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

#define MAX_DIGEST_SIZE 40

/* Key generation using OpenPGP Simple S2K algorithm */
int _mhash_gen_key_s2k_simple(hashid algorithm, void *keyword, int key_size,
		  unsigned char *password, int plen)
{
	word8* key;
	word8 digest[MAX_DIGEST_SIZE];
	char null='\0';
	int i,j, times;
	MHASH td;
	int block_size = mhash_get_block_size(algorithm);


	times = key_size/block_size;
	if (key_size%block_size != 0) times++;

	if( (key = calloc(1, times*block_size) ) == NULL)
		return -1; /* or what? */

	
	for (i=0;i<times;i++) {
		td = mhash_init(algorithm);
		if (td==MHASH_FAILED) {
			free(key);
			return -1;
		}
		
		for (j=0;j<i;j++)
			mhash(td, &null, 1);
		mhash(td, password, plen);
		mhash_deinit(td, digest);

		memcpy( &key[i*block_size], digest, block_size);
	}
	memcpy(keyword, key, key_size);
	mhash_bzero(key, key_size);
	free(key);
	return 0;
}


/* Key generation using OpenPGP Salted S2K algorithm */
int _mhash_gen_key_s2k_salted(hashid algorithm, void *keyword, int key_size,
		  unsigned char* salt, int salt_size,
		  unsigned char *password, int plen)
{
	word8* key;
	word8 digest[MAX_DIGEST_SIZE];
	char null='\0';
	int i,j, times;
	MHASH td;
	int block_size = mhash_get_block_size(algorithm);

	if (salt==NULL) return -1;
	if (salt_size<8) return -1; /* This algorithm will use EXACTLY
				     * 8 bytes salt.
				     */
	times = key_size/block_size;
	if (key_size%block_size != 0) times++;

	if((key=calloc(1, times*block_size)) == NULL)
		return -1; /* or what? */

	
	for (i=0;i<times;i++) {
		td = mhash_init(algorithm);
		if (td==MHASH_FAILED) {
			free(key);
			return -1;
		}
		
		for (j=0;j<i;j++)
			mhash(td, &null, 1);

		mhash(td, salt, 8);
		mhash(td, password, plen);
		mhash_deinit(td, digest);
		
		memcpy( &key[i*block_size], digest, block_size);
	}
	memcpy(keyword, key, key_size);
	mhash_bzero(key, key_size);
	free(key);
	return 0;
}

#define EXPBIAS 6
           
/* Key generation using OpenPGP Iterated and Salted S2K algorithm */
int _mhash_gen_key_s2k_isalted(hashid algorithm, unsigned long _count, 
		  void *keyword, int key_size,
		  unsigned char* salt, int salt_size,
		  unsigned char *password, int plen)
{
	word8* key;
	word8 digest[MAX_DIGEST_SIZE];
	char null='\0';
	int i,j, z, times;
	MHASH td;
	int block_size = mhash_get_block_size(algorithm);
	char* saltpass;
	int saltpass_size;
	word32 bcount, rest;
	word32 count = _count;

	if (salt==NULL) return -1;
	if (salt_size<8) return -1; /* This algorithm will use EXACTLY
				     * 8 bytes salt.
				     */

	if((saltpass = calloc(1, 8+plen)) == NULL) return -1; /* hmm */
	memcpy( saltpass, salt, 8);
	memcpy( &saltpass[8], password, plen);
	saltpass_size = plen+8;

	times = key_size/block_size;
	if (key_size%block_size != 0) times++;
	if ( (key=calloc(1, times*block_size))==NULL) {
	   mhash_bzero(saltpass, saltpass_size);
	   free( saltpass);
	   return -1;
        }

	/* Calculate the iterations
	 */
	bcount /*bytes */ = 
		((word32)16 + (count & 15)) << ((count >> 4) + EXPBIAS); 

	count /* iterations */ = 
		(bcount / saltpass_size);

	rest = bcount % saltpass_size;
	if (bcount < saltpass_size) {
		count++;
		rest = 0;
	}
	
	for (i=0;i<times;i++) {
		td = mhash_init(algorithm);
		if (td==MHASH_FAILED) {
		        mhash_bzero( key, key_size);
		        mhash_bzero( saltpass, saltpass_size);
			free(key);
			free(saltpass);
			return -1;
		}
	
		for (j=0;j<i;j++)
			mhash(td, &null, 1);

		for (z=0;z<count;z++) {
			mhash(td, saltpass, saltpass_size);
		}
		mhash( td, saltpass, rest);

		mhash_deinit(td, digest);
		
		memcpy( &key[i*block_size], digest, block_size);
	}
	memcpy(keyword, key, key_size);

	mhash_bzero(key, key_size);
	mhash_bzero(saltpass, saltpass_size);

	free(key);
	free(saltpass);

	return 0;
}
