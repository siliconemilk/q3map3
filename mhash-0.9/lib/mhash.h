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


#ifndef MHASH_H
#define MHASH_H

/* $Id: mhash.h,v 1.20 2002/05/24 16:40:59 nmav Exp $ */

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#ifdef WIN32
#include <memory.h> /* size_t */
# define WIN32DLL_DEFINE __declspec( dllexport)
#else
# define WIN32DLL_DEFINE
#endif

#define MHASH_API_VERSION 20020524

/* these are for backwards compatibility and will 
   be removed at some time */
#ifdef MHASH_BACKWARDS_COMPATIBLE
# define MHASH_HAVAL MHASH_HAVAL256
# define MHASH_TIGER192 MHASH_TIGER
# define hmac_mhash_init mhash_hmac_init
# define hmac_mhash_end mhash_hmac_end
#endif

/* typedefs */ struct MHASH_INSTANCE;
	typedef struct MHASH_INSTANCE *MHASH;

	enum hashid {
		MHASH_CRC32,
		MHASH_MD5,
		MHASH_SHA1,
		MHASH_HAVAL256,	/* 3 passes */
		MHASH_RIPEMD160 = 5,
		MHASH_TIGER = 7,
		MHASH_GOST,
		MHASH_CRC32B,
		MHASH_HAVAL224 = 10,
		MHASH_HAVAL192,
		MHASH_HAVAL160,
		MHASH_HAVAL128,
		MHASH_TIGER128,
		MHASH_TIGER160,
		MHASH_MD4,
		MHASH_SHA256,
		MHASH_ADLER32,
		MHASH_SHA224,
		MHASH_SHA512,
		MHASH_SHA384,
		MHSH_WHIRLPOOL,
		MHASH_RIPEMD128,
		MHASH_RIPEMD256,
		MHASH_RIPEMD320,
	};

	enum keygenid {
		KEYGEN_MCRYPT,	/* The key generator used in mcrypt */
		KEYGEN_ASIS,	/* Just returns the password as binary key */
		KEYGEN_HEX,	/* Just converts a hex key into a binary one */
		KEYGEN_PKDES,	/* The transformation used in Phil Karn's DES
				 * encryption program */
		KEYGEN_S2K_SIMPLE,	/* The OpenPGP (rfc2440) Simple S2K */
		KEYGEN_S2K_SALTED,	/* The OpenPGP Salted S2K */
		KEYGEN_S2K_ISALTED	/* The OpenPGP Iterated Salted S2K */
	};

	typedef enum hashid hashid;
	typedef enum keygenid keygenid;

	typedef struct mhash_hash_entry mhash_hash_entry;

	typedef struct keygen {
		hashid 		hash_algorithm[2];
		unsigned int	count;
		void*		salt;
		int		salt_size;
	} KEYGEN;


#define MHASH_FAILED ((MHASH) 0x0)

/* information prototypes */

	WIN32DLL_DEFINE size_t mhash_count(void);
	WIN32DLL_DEFINE size_t mhash_get_block_size(hashid type);
	WIN32DLL_DEFINE char *mhash_get_hash_name(hashid type);
	WIN32DLL_DEFINE const char *mhash_get_hash_name_static(hashid type);
	WIN32DLL_DEFINE void mhash_free(void *ptr);

/* initializing prototypes */

	WIN32DLL_DEFINE MHASH mhash_init(hashid type);

/* copy prototypes */

	WIN32DLL_DEFINE MHASH mhash_cp(MHASH);

/* update prototype */

	WIN32DLL_DEFINE int mhash(MHASH thread, const void *plaintext, size_t size);

/* finalizing prototype */

	WIN32DLL_DEFINE void *mhash_end(MHASH thread);
	WIN32DLL_DEFINE void *mhash_end_m(MHASH thread, void *(*hash_malloc) (size_t));
	WIN32DLL_DEFINE void mhash_deinit(MHASH thread, void *result);

/* informational */
	WIN32DLL_DEFINE size_t mhash_get_hash_pblock(hashid type);

	WIN32DLL_DEFINE hashid mhash_get_mhash_algo(MHASH);

/* HMAC */
	WIN32DLL_DEFINE MHASH mhash_hmac_init(const hashid type, void *key, int keysize,
			      int block);
	WIN32DLL_DEFINE void *mhash_hmac_end_m(MHASH thread, void *(*hash_malloc) (size_t));
	WIN32DLL_DEFINE void *mhash_hmac_end(MHASH thread);
	WIN32DLL_DEFINE int mhash_hmac_deinit(MHASH thread, void *result);

/* Save state functions */
	WIN32DLL_DEFINE int mhash_save_state_mem(MHASH thread, void *mem, int* mem_size );
	WIN32DLL_DEFINE MHASH mhash_restore_state_mem(void* mem);

/* Key generation functions */
	WIN32DLL_DEFINE int mhash_keygen(keygenid algorithm, hashid opt_algorithm,
			 unsigned long count, void *keyword, int keysize,
			 void *salt, int saltsize, unsigned char *password,
			 int passwordlen);
	WIN32DLL_DEFINE int mhash_keygen_ext(keygenid algorithm, KEYGEN data,
		 void *keyword, int keysize,
		 unsigned char *password, int passwordlen);

	WIN32DLL_DEFINE char *mhash_get_keygen_name(keygenid type);
	WIN32DLL_DEFINE const char *mhash_get_keygen_name_static(hashid type);

	WIN32DLL_DEFINE size_t mhash_get_keygen_salt_size(keygenid type);
	WIN32DLL_DEFINE size_t mhash_get_keygen_max_key_size(keygenid type);
	WIN32DLL_DEFINE size_t mhash_keygen_count(void);
	WIN32DLL_DEFINE int mhash_keygen_uses_salt(keygenid type);
	WIN32DLL_DEFINE int mhash_keygen_uses_count(keygenid type);
	WIN32DLL_DEFINE int mhash_keygen_uses_hash_algorithm(keygenid type);


#ifdef __cplusplus
}
#endif
#endif				/* !MHASH_H */
