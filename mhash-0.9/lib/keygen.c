/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000,2001 Sascha Schumman, Nikos Mavroyanopoulos
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



#include "libdefs.h"
#include "mhash_int.h"
#include "keygen.h"

#define KEYGEN_ENTRY(name, uses_hash_algorithm, uses_count, uses_salt, salt_size, max_key_size) \
        { #name, name, uses_hash_algorithm, uses_count, uses_salt, salt_size, max_key_size }

typedef struct mhash_keygen_entry {
	char *name;
	keygenid id;
	int uses_hash_algorithm;
	int uses_count;
	int uses_salt;
	size_t salt_size;
	size_t max_key_size;
} mhash_keygen_entry;

static const mhash_keygen_entry keygen_algorithms[] = {
	KEYGEN_ENTRY(KEYGEN_ASIS, 0, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_PKDES, 0, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_HEX, 0, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_MCRYPT, 1, 0, 1, 0, 0),
	KEYGEN_ENTRY(KEYGEN_S2K_SIMPLE, 1, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_S2K_SALTED, 1, 0, 1, 8, 0),
	KEYGEN_ENTRY(KEYGEN_S2K_ISALTED, 1, 1, 1, 8, 0),
	{0}
};

#define KEYGEN_LOOP(b) \
        const mhash_keygen_entry *p; \
                for(p = keygen_algorithms; p->name != NULL; p++) { b ; }

#define KEYGEN_ALG_LOOP(a) \
                        KEYGEN_LOOP( if(p->id == type) { a; break; } )

WIN32DLL_DEFINE
int mhash_keygen_uses_hash_algorithm(keygenid type)
{
	int ret = 0;
	KEYGEN_ALG_LOOP(ret = p->uses_hash_algorithm);
	return ret;
}


WIN32DLL_DEFINE
int mhash_keygen_uses_count(keygenid type)
{
	int ret = 0;
	KEYGEN_ALG_LOOP(ret = p->uses_count);
	return ret;
}


WIN32DLL_DEFINE
int mhash_keygen_uses_salt(keygenid type)
{
	int ret = 0;
	KEYGEN_ALG_LOOP(ret = p->uses_salt);
	return ret;
}

#ifndef MMAX
# define MMAX(x,y) (x>y?x:y)
#endif

WIN32DLL_DEFINE
size_t mhash_keygen_count(void)
{
	keygenid count = 0;

	KEYGEN_LOOP(count = MMAX(p->id, count) );

	return count;
}

WIN32DLL_DEFINE
size_t mhash_get_keygen_salt_size(keygenid type)
{
	size_t ret = 0;

	KEYGEN_ALG_LOOP(ret = p->salt_size);
	return ret;
}

WIN32DLL_DEFINE
size_t mhash_get_keygen_max_key_size(keygenid type)
{
	size_t ret = 0;

	KEYGEN_ALG_LOOP(ret = p->max_key_size);
	return ret;
}

WIN32DLL_DEFINE
char *mhash_get_keygen_name(hashid type)
{
	char *ret = NULL;

	/* avoid prefix */
	KEYGEN_ALG_LOOP(ret = p->name);
	if ( ret!=NULL) ret += sizeof("KEYGEN_") - 1;

	return mystrdup(ret);
}

WIN32DLL_DEFINE
const char *mhash_get_keygen_name_static(hashid type)
{
	char *ret = NULL;

	/* avoid prefix */
	KEYGEN_ALG_LOOP(ret = p->name);
	if (ret!=NULL) ret += sizeof("KEYGEN_") - 1;

	return ret;
}


WIN32DLL_DEFINE
int mhash_keygen(keygenid algorithm, hashid opt_algorithm,
		 unsigned long count, void *keyword, int keysize,
		 void *salt, int saltsize, unsigned char *password,
		 int passwordlen)
{
int x;

	switch (algorithm) {

	case KEYGEN_MCRYPT:
		x=_mhash_gen_key_mcrypt(opt_algorithm, keyword, keysize,
				      salt, saltsize, password,
				      passwordlen);
		break;
	case KEYGEN_ASIS:
		x=_mhash_gen_key_asis(keyword, keysize, password,
				    passwordlen);
		break;
	case KEYGEN_PKDES:
		x=_mhash_gen_key_pkdes(keyword, keysize, password,
				    passwordlen);
		break;
	case KEYGEN_HEX:
		x=_mhash_gen_key_hex(keyword, keysize, password,
				   passwordlen);
		break;
	case KEYGEN_S2K_SIMPLE:
		x=_mhash_gen_key_s2k_simple(opt_algorithm, keyword, keysize,
					  password, passwordlen);
		break;
	case KEYGEN_S2K_SALTED:
		x=_mhash_gen_key_s2k_salted(opt_algorithm, keyword, keysize,
					  salt, saltsize, password,
					  passwordlen);
		break;
	case KEYGEN_S2K_ISALTED:
		x=_mhash_gen_key_s2k_isalted(opt_algorithm, count, keyword,
					   keysize, salt, saltsize,
					   password, passwordlen);
		break;
	default:
		return -1;
	}

	return x;
}



WIN32DLL_DEFINE
int mhash_keygen_ext(keygenid algorithm, KEYGEN data,
		 void *keyword, int keysize,
		 unsigned char *password, int passwordlen)
{
int x;
hashid opt_algorithm = data.hash_algorithm[0];
unsigned int count = data.count;
void* salt = data.salt;
int saltsize= data.salt_size;

	switch (algorithm) {

	case KEYGEN_MCRYPT:
		x=_mhash_gen_key_mcrypt(opt_algorithm, keyword, keysize,
				      salt, saltsize, password,
				      passwordlen);
		break;
	case KEYGEN_ASIS:
		x=_mhash_gen_key_asis(keyword, keysize, password,
				    passwordlen);
		break;
	case KEYGEN_PKDES:
		x=_mhash_gen_key_pkdes(keyword, keysize, password,
				    passwordlen);
		break;
	case KEYGEN_HEX:
		x=_mhash_gen_key_hex(keyword, keysize, password,
				   passwordlen);
		break;
	case KEYGEN_S2K_SIMPLE:
		x=_mhash_gen_key_s2k_simple(opt_algorithm, keyword, keysize,
					  password, passwordlen);
		break;
	case KEYGEN_S2K_SALTED:
		x=_mhash_gen_key_s2k_salted(opt_algorithm, keyword, keysize,
					  salt, saltsize, password,
					  passwordlen);
		break;
	case KEYGEN_S2K_ISALTED:
		x=_mhash_gen_key_s2k_isalted(opt_algorithm, count, keyword,
					   keysize, salt, saltsize,
					   password, passwordlen);
		break;
	default:
		return -1;
	}

	return x;
}
