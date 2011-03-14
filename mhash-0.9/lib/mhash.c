/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
 *    Copyright (C) 2001 Nikos Mavroyanopoulos
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


/* $Id: mhash.c,v 1.34 2003/01/19 18:00:58 nmav Exp $ */

#include <stdlib.h>

#include "libdefs.h"

#include "mhash_int.h"

#ifdef ENABLE_CRC32
# include "mhash_crc32.h"
#endif

#ifdef ENABLE_HAVAL
# include "mhash_haval.h"
#endif

#ifdef ENABLE_MD5
# include "mhash_md5.h"
#endif

#ifdef ENABLE_MD4
# include "mhash_md4.h"
#endif

#ifdef ENABLE_SHA1
# include "mhash_sha1.h"
#endif

#ifdef ENABLE_TIGER
# include "mhash_tiger.h"
#endif

#ifdef ENABLE_RIPEMD
# include "mhash_ripemd.h"
#endif

#ifdef ENABLE_SHA256_SHA224
# include "mhash_sha256_sha224.h"
#endif

#ifdef ENABLE_SHA512_SHA384
# include "mhash_sha512_sha384.h"
#endif

#ifdef ENABLE_ADLER32
# include "mhash_adler32.h"
#endif

#ifdef ENABLE_GOST
# include "mhash_gost.h"
#endif

#ifdef ENABLE_WHIRLPOOL
# include "mhash_whirlpool.h"
#endif

/* 19/03/2000 Changes for better thread handling --nikos 
 * Actually it is thread safe.
 */

#define MAX_BLOCK_SIZE 128

#define MHASH_ENTRY(name, blksize, hash_pblock, state_size, init_func, \
	hash_func, final_func, deinit_func) \
	{ #name, name, blksize, hash_pblock, state_size, init_func,\
		hash_func, final_func, deinit_func }

struct mhash_hash_entry {
	char *name;
	hashid id;
	size_t blocksize;
	size_t hash_pblock;

	size_t state_size;
	INIT_FUNC init_func;
	HASH_FUNC hash_func;
	FINAL_FUNC final_func;
	DEINIT_FUNC deinit_func;
};

static const mhash_hash_entry algorithms[] = {
#ifdef ENABLE_CRC32
	MHASH_ENTRY(MHASH_CRC32, 4, 0, sizeof(word32), mhash_clear_crc32, 
		mhash_crc32, NULL, mhash_get_crc32),
	MHASH_ENTRY(MHASH_CRC32B, 4, 0, sizeof(word32), mhash_clear_crc32,
		mhash_crc32b, NULL, mhash_get_crc32),
#endif

#ifdef ENABLE_ADLER32
	MHASH_ENTRY(MHASH_ADLER32, 4, 0, sizeof(word32), mhash_clear_adler32, 
		mhash_adler32, NULL, mhash_get_adler32),
#endif

#ifdef ENABLE_MD5
	MHASH_ENTRY(MHASH_MD5, 16, 64, sizeof(MD5_CTX), MD5Init,
		MD5Update, NULL, MD5Final),
#endif

#ifdef ENABLE_MD4
	MHASH_ENTRY(MHASH_MD4, 16, 64, sizeof(MD4_CTX), MD4Init, 
		MD4Update, NULL, MD4Final),
#endif

#ifdef ENABLE_SHA1
	MHASH_ENTRY(MHASH_SHA1, 20, 64, sizeof(SHA_CTX), sha_init, 
		sha_update, sha_final, sha_digest),
#endif

#ifdef ENABLE_SHA256_SHA224
	MHASH_ENTRY(MHASH_SHA256, 32, 64, sizeof( SHA256_SHA224_CTX), sha256_init,
		sha256_sha224_update, sha256_sha224_final, sha256_digest),
	MHASH_ENTRY(MHASH_SHA224, 28, 64, sizeof( SHA256_SHA224_CTX), sha224_init,
		sha256_sha224_update, sha256_sha224_final, sha224_digest),
#endif

#ifdef ENABLE_SHA512_SHA384
	MHASH_ENTRY(MHASH_SHA512, 64, 128, sizeof( SHA512_SHA384_CTX), sha512_init,
		sha512_sha384_update, sha512_sha384_final, sha512_digest),
	MHASH_ENTRY(MHASH_SHA384, 48, 128, sizeof( SHA512_SHA384_CTX), sha384_init,
		sha512_sha384_update, sha512_sha384_final, sha384_digest),
#endif

#ifdef ENABLE_HAVAL
	MHASH_ENTRY(MHASH_HAVAL256, 32, 128, sizeof(havalContext), havalInit256,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL128, 16, 128, sizeof(havalContext), havalInit128,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL160, 20, 128, sizeof(havalContext), havalInit160,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL192, 24, 128, sizeof(havalContext), havalInit192,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL224, 28, 128, sizeof(havalContext), havalInit224,
		havalUpdate, NULL, havalFinal),
#endif

#ifdef ENABLE_RIPEMD
	MHASH_ENTRY(MHASH_RIPEMD128, 16, 64, sizeof(RIPEMD_CTX), ripemd128_init, 
		ripemd_update, ripemd_final, ripemd_digest),
	MHASH_ENTRY(MHASH_RIPEMD160, 20, 64, sizeof(RIPEMD_CTX), ripemd160_init, 
		ripemd_update, ripemd_final, ripemd_digest),
	MHASH_ENTRY(MHASH_RIPEMD256, 32, 64, sizeof(RIPEMD_CTX), ripemd256_init, 
		ripemd_update, ripemd_final, ripemd_digest),
	MHASH_ENTRY(MHASH_RIPEMD320, 40, 64, sizeof(RIPEMD_CTX), ripemd320_init, 
		ripemd_update, ripemd_final, ripemd_digest),
#endif

#ifdef ENABLE_TIGER
	MHASH_ENTRY(MHASH_TIGER, 24, 64, sizeof(TIGER_CTX), tiger_init, 
		tiger_update, tiger_final, tiger_digest),
	MHASH_ENTRY(MHASH_TIGER128, 16, 64, sizeof(TIGER_CTX), tiger_init,
		tiger_update, tiger_final, tiger128_digest),
	MHASH_ENTRY(MHASH_TIGER160, 20, 64, sizeof(TIGER_CTX), tiger_init, 
		tiger_update, tiger_final, tiger160_digest),
#endif

#ifdef ENABLE_GOST
	MHASH_ENTRY(MHASH_GOST, 32, 0, sizeof(GostHashCtx), gosthash_reset, 
		gosthash_update, NULL, gosthash_final),
#endif

#ifdef ENABLE_WHIRLPOOL
	MHASH_ENTRY(MHASH_WHIRLPOOL, 64, 64, sizeof(WHIRLPOOL_CTX), whirlpool_init, 
		    whirlpool_update, whirlpool_final, whirlpool_digest),
#endif
	{0}
};

#define MHASH_LOOP(b) \
	const mhash_hash_entry *p; \
	for(p = algorithms; p->name != NULL; p++) { b ; }

#define MHASH_ALG_LOOP(a) \
	MHASH_LOOP( if(p->id == type) { a; break; } )

#ifndef MMAX
# define MMAX(x,y) (x>y?x:y)
#endif

WIN32DLL_DEFINE size_t mhash_count(void)
{
	hashid count = 0;

	MHASH_LOOP( count = MMAX( p->id, count) );

	return count;
}

WIN32DLL_DEFINE size_t mhash_get_block_size(hashid type)
{
	size_t ret = 0;

	MHASH_ALG_LOOP(ret = p->blocksize);
	return ret;
}

WIN32DLL_DEFINE int _mhash_get_state_size(hashid type)
{
	int size = -1;

	MHASH_ALG_LOOP(size = p->state_size);
	return size;
}
WIN32DLL_DEFINE INIT_FUNC _mhash_get_init_func(hashid type)
{
	INIT_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->init_func);
	return func;
}
WIN32DLL_DEFINE HASH_FUNC _mhash_get_hash_func(hashid type)
{
	HASH_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->hash_func);
	return func;
}
WIN32DLL_DEFINE FINAL_FUNC _mhash_get_final_func(hashid type)
{
	FINAL_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->final_func);
	return func;
}
WIN32DLL_DEFINE DEINIT_FUNC _mhash_get_deinit_func(hashid type)
{
	DEINIT_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->deinit_func);
	return func;
}


/* function created in order for mhash to compile under WIN32 */
char *mystrdup(char *str)
{
	char *ret;
	if (str==NULL) return NULL;
	
	if ( (ret = malloc(strlen(str) + 1)) == NULL) return NULL;
	strcpy(ret, str);

	return ret;

}

WIN32DLL_DEFINE hashid mhash_get_mhash_algo( MHASH tmp) {
	return tmp->algorithm_given;
}

WIN32DLL_DEFINE char *mhash_get_hash_name(hashid type)
{
	char *ret = NULL;

	/* avoid prefix */
	MHASH_ALG_LOOP( ret = p->name);

	if (ret!=NULL) ret += sizeof("MHASH_") - 1;
	
	return mystrdup(ret);
}

WIN32DLL_DEFINE const char *mhash_get_hash_name_static(hashid type)
{
	char *ret = NULL;

	/* avoid prefix */
	MHASH_ALG_LOOP( ret = p->name);
	
	if ( ret !=NULL) ret+= sizeof("MHASH_") - 1;

	return ret;
}

MHASH mhash_cp(MHASH from) {
MHASH ret;

	if ( (ret = malloc(sizeof(MHASH_INSTANCE))) == NULL) return MHASH_FAILED;
	memcpy(ret, from, sizeof(MHASH_INSTANCE));
	
	/* copy the internal state also */
	if ( (ret->state=malloc(ret->state_size)) == NULL) return MHASH_FAILED;
	memcpy( ret->state, from->state, ret->state_size);
	
	/* copy the key in case of hmac*/
	if (ret->hmac_key_size!=0) {
		if ((ret->hmac_key=malloc(ret->hmac_key_size)) == NULL) return MHASH_FAILED;
		memcpy(ret->hmac_key, from->hmac_key, ret->hmac_key_size);
	}
	return ret;

}
MHASH mhash_init_int(const hashid type)
{
	MHASH ret;
	INIT_FUNC func;

	if ( (ret = malloc( sizeof(MHASH_INSTANCE))) == NULL) return MHASH_FAILED;
	ret->algorithm_given = type;
	ret->hmac_key = NULL;
	ret->state = NULL;
	ret->hmac_key_size = 0;

	ret->state_size = _mhash_get_state_size( type);
	if (ret->state_size <= 0) {
		free(ret);
		return MHASH_FAILED;
	}
	
	if ( (ret->state = malloc(ret->state_size)) == NULL) {
		free(ret);
		return MHASH_FAILED;
	}
	func = _mhash_get_init_func( type);
	if (func!=NULL)
		func( ret->state);
	else {
		free( ret);
		return MHASH_FAILED;
	}

	ret->hash_func = _mhash_get_hash_func( type);
	ret->deinit_func = _mhash_get_deinit_func( type);
	ret->final_func = _mhash_get_final_func( type);

	return ret;
		
}

#define MIX32(a) \
	(((unsigned long)((unsigned char *)(a))[0]) | \
	(((unsigned long)((unsigned char *)(a))[1]) << 8)| \
	(((unsigned long)((unsigned char *)(a))[2]) << 16)| \
	(((unsigned long)((unsigned char *)(a))[3]) << 24))


#ifdef WORDS_BIGENDIAN
void mhash_32bit_conversion(word32 * ptr, size_t count)
{
	size_t i;

	for (i = 0; i < count; i++) {
		ptr[i] = MIX32(&ptr[i]);
	}
}
#else
#define mhash_32bit_conversion(a,b)
#endif

/* plaintext should be a multiply of the algorithm's block size */

int mhash(MHASH td, const void *plaintext, size_t size)
{
	
	if (td->hash_func!=NULL)
		td->hash_func( td->state, plaintext, size);

	return 0;
}


WIN32DLL_DEFINE
    void mhash_deinit(MHASH td, void *result)
{
	
	if (td->final_func!=NULL)
		td->final_func( td->state);

	if (td->deinit_func!=NULL)
		td->deinit_func( td->state, result);

	if (NULL != td->state) {
		free(td->state);
	}
	free(td);

	return;
}

WIN32DLL_DEFINE
    void *mhash_end_m(MHASH td, void *(*hash_malloc) (size_t))
{
	void *digest;
	int size;

	size = mhash_get_block_size( td->algorithm_given);
	
	digest = hash_malloc( size);
	if (digest==NULL) return NULL;
	
	mhash_deinit( td, digest);
	
	return digest;
}

WIN32DLL_DEFINE void *mhash_end(MHASH td)
{
	return mhash_end_m(td, malloc);
}


WIN32DLL_DEFINE MHASH mhash_init(const hashid type)
{
	MHASH ret = MHASH_FAILED;

	ret = mhash_init_int(type);

	return ret;
}

/* HMAC functions */

WIN32DLL_DEFINE size_t mhash_get_hash_pblock(hashid type)
{
	size_t ret = 0;

	MHASH_ALG_LOOP(ret = p->hash_pblock);
	return ret;
}

WIN32DLL_DEFINE
    int mhash_hmac_deinit(MHASH td, void *result)
{
	unsigned char *opad;
	unsigned char _opad[MAX_BLOCK_SIZE];
	MHASH tmptd;
	int i, opad_alloc = 0;
	


	if (td->hmac_block > MAX_BLOCK_SIZE) {
		opad = malloc(td->hmac_block);
		if (opad == NULL) return -1;
		opad_alloc = 1;
	} else {
		opad = _opad;
	}


	for (i = 0; i < td->hmac_key_size; i++) {
		opad[i] = (0x5C) ^ td->hmac_key[i];
	}
	for (; i < td->hmac_block; i++) {
		opad[i] = (0x5C);
	}

	tmptd = mhash_init(td->algorithm_given);
	mhash(tmptd, opad, td->hmac_block);

	if (td->final_func!=NULL)
		td->final_func( td->state);

	if (td->deinit_func!=NULL)
		td->deinit_func( td->state, result);

	if (result!=NULL)
		mhash(tmptd, result,
		      mhash_get_block_size(td->algorithm_given));

	free(td->state);

	if (opad_alloc!=0) free(opad);
	
	mhash_bzero(td->hmac_key, td->hmac_key_size);
	free(td->hmac_key);
	free(td);

	mhash_deinit(tmptd, result);

	return 0;
}


WIN32DLL_DEFINE
    void *mhash_hmac_end_m(MHASH td, void *(*hash_malloc) (size_t))
{
	void *digest;

	digest =
	    hash_malloc(mhash_get_block_size
			(td->algorithm_given));
	if (digest == NULL) return NULL;

	mhash_hmac_deinit( td, digest);
	
	return digest;
	
}

WIN32DLL_DEFINE void *mhash_hmac_end(MHASH td)
{
	return mhash_hmac_end_m(td, malloc);
}


WIN32DLL_DEFINE
    MHASH mhash_hmac_init(const hashid type, void *key, int keysize,
			  int block)
{
	MHASH ret = MHASH_FAILED;
	MHASH tmptd;
	unsigned char *ipad;
	unsigned char _ipad[MAX_BLOCK_SIZE];
	int i, ipad_alloc=0;

	if (block == 0) {
		block = 64;	/* the default for ripemd,md5,sha-1 */
	}

	ret = mhash_init_int(type);

	if (ret != MHASH_FAILED) {
		/* Initial hmac calculations */
		ret->hmac_block = block;

		if ( ret->hmac_block > MAX_BLOCK_SIZE) {
			ipad = malloc(ret->hmac_block);
			if (ipad == NULL) return MHASH_FAILED;
			ipad_alloc = 1;
		} else {
			ipad = _ipad;
		}
		
		if (keysize > ret->hmac_block) {
			tmptd = mhash_init(type);
			mhash(tmptd, key, keysize);
			ret->hmac_key_size = mhash_get_block_size(type);
			ret->hmac_key = mhash_end(tmptd);
		} else {
			ret->hmac_key = calloc(1, ret->hmac_block);
			memcpy(ret->hmac_key, key, keysize);
			ret->hmac_key_size = ret->hmac_block;
		}

		/* IPAD */

		for (i = 0; i < ret->hmac_key_size; i++) {
			ipad[i] = (0x36) ^ ret->hmac_key[i];
		}
		for (; i < ret->hmac_block; i++) {
			ipad[i] = (0x36);
		}

		mhash(ret, ipad, ret->hmac_block);

		if (ipad_alloc!=0) free(ipad);
	}


	return ret;
}

WIN32DLL_DEFINE void mhash_free(void *ptr)
{
	free(ptr);
}

/*
  Saves the state of a hashing algorithm such that it can be
  restored at some later point in time using
  mhash_restore_state().
  
  mem_size should contain the size of the given _mem pointer.
  Otherwise the required size will be copied there.

  Original version and idea by Blake Stephen <Stephen.Blake@veritect.com>
*/
WIN32DLL_DEFINE int mhash_save_state_mem(MHASH td, void *_mem, int* mem_size )
{
	int tot_size, pos;
	unsigned char* mem = _mem;
	
	tot_size = sizeof(td->algorithm_given) + sizeof(td->hmac_key_size)
		+ sizeof(td->hmac_block) + td->hmac_key_size +
		+ sizeof(td->state_size) + td->state_size;
	
	if ( *mem_size < tot_size) {
		*mem_size = tot_size;
		return -1;
	}
	
	if ( mem != NULL) {
		pos = 0;
		memcpy( mem, &td->algorithm_given, sizeof(td->algorithm_given));
		pos = sizeof( td->algorithm_given);
		
		memcpy( &mem[pos], &td->hmac_key_size, sizeof(td->hmac_key_size));
		pos += sizeof(td->hmac_key_size);

		memcpy( &mem[pos], &td->hmac_block, sizeof(td->hmac_block));
		pos += sizeof(td->hmac_block);

		memcpy( &mem[pos], td->hmac_key, td->hmac_key_size);
		pos += td->hmac_key_size;

		memcpy( &mem[pos], &td->state_size, sizeof(td->state_size));
		pos += sizeof(td->state_size);

		memcpy( &mem[pos], td->state, td->state_size);
		pos += td->state_size;

	}
	return 0;
}


/*
  Restores the state of a hashing algorithm that was saved
  using mhash_save_state(). Use like mhash_init.
*/
WIN32DLL_DEFINE MHASH mhash_restore_state_mem(void* _mem)
{
	unsigned char* mem = _mem;
	hashid algorithm_given;
	MHASH ret = MHASH_FAILED;
	int pos;

	if (mem==NULL)
		return ret;

	memcpy( &algorithm_given, mem, sizeof(algorithm_given));
	
	if ((ret = mhash_init(algorithm_given)) == MHASH_FAILED) {
		return ret;
	}

	ret->algorithm_given = algorithm_given;
	
	pos = sizeof(algorithm_given);

 	memcpy( &ret->hmac_key_size, &mem[pos], sizeof(ret->hmac_key_size));
	pos += sizeof( ret->hmac_key_size);
		
	memcpy( &ret->hmac_block, &mem[pos], sizeof(ret->hmac_block));
	pos += sizeof(ret->hmac_block);

	if (ret->hmac_key_size != 0) {
		ret->hmac_key = malloc( ret->hmac_key_size);
		if (ret->hmac_key==NULL) goto freeall;
	
		memcpy( ret->hmac_key, &mem[pos], ret->hmac_key_size);
		pos += sizeof(ret->hmac_key_size);
	}

	memcpy( &ret->state_size, &mem[pos], sizeof(ret->state_size));
	pos += sizeof( ret->state_size);

	ret->state = malloc( ret->state_size);
	if (ret->state==NULL) goto freeall;

	memcpy( ret->state, &mem[pos], ret->state_size);
	pos += ret->state_size;

	ret->hash_func = _mhash_get_hash_func( algorithm_given);
	ret->deinit_func = _mhash_get_deinit_func( algorithm_given);
	ret->final_func = _mhash_get_final_func( algorithm_given);

	return ret;

	freeall:
		/* This uses too much internals
		 */
		free( ret->state);
		free( ret->hmac_key);
		free( ret);
		return MHASH_FAILED;
}

#ifdef WIN32
WIN32DLL_DEFINE int main (void)
{
       /* empty main function to avoid linker error (see cygwin FAQ) */
}
#endif
