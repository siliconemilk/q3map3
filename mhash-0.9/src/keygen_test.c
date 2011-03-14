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



#include "../lib/mhash.h"
#include <stdio.h>
#include <stdlib.h>

#define PASSWORD1 "Hello there"
#define KEY1 "95686ac64b9d18e71e3f41445ef0e5cbec88445e83aa03a8255cb5aa234d6f8c7bd6bfc2f2eb6051c29658cfb77cd0fc1a8254612193e3ec64b38a77803eee54788421b72508372a1363e4761a83c5775bdd2edd8dc12ba4321a4a113a73902eac824ac9"

#define PASSWORD2 "One two three four"
#define KEY2 "7208aa08e528ca3ff036ca369fc067bc0aea65b8868aae3dccd9b5f25c5a23c4f074e9a421ce5f78107d820335f2f7b38ca114080b5ec47c52d7c4a18c1f79b32b258172b7c13a0f50b929fed354dc9bab4a4eea13b7fe58675624ed66095bc2b7a925a3"

int main()
{

	char *tmp;
	char tmp2[3];
	char *password, *salt;
	int passlen;
	int keysize, salt_size;
	KEYGEN data;
	unsigned char *key;
	int j;

	passlen=sizeof(PASSWORD1);
	password = malloc(passlen+1);
	strncpy(password, PASSWORD1, passlen);
	
	if (mhash_get_keygen_max_key_size(KEYGEN_MCRYPT)==0) {
		keysize=100;
	} else {
		keysize = mhash_get_keygen_max_key_size(KEYGEN_MCRYPT);
	}

	if (mhash_get_keygen_salt_size(KEYGEN_MCRYPT)==0) {
		salt_size=10;
	} else {
		salt_size = mhash_get_keygen_salt_size(KEYGEN_MCRYPT);
	}
	
	salt = calloc( 1, salt_size);
	key = calloc( 1, keysize);
	tmp = calloc( 1, 2*keysize+1);
	
	data.hash_algorithm[0] = MHASH_MD5;
	data.count = 0;
	data.salt = salt;
	data.salt_size = salt_size;
	
	mhash_keygen_ext(KEYGEN_MCRYPT, data, key, keysize, password, passlen);

	for (j = 0; j < keysize; j++) {
		sprintf(tmp2, "%.2x", key[j]);
		strcat(tmp, tmp2);
	}

	if (strcmp(KEY1, tmp)!=0) {
		fprintf(stderr, "KEYGEN-Test (KEYGEN_MCRYPT): Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", KEY1, tmp);
		free(password);
		free(tmp);
		free(key);
		return 1;
	}

	free(password);
	free(key);
	free(tmp);


	passlen=sizeof(PASSWORD2);
	password = malloc(passlen+1);
	strncpy(password, PASSWORD2, passlen);
	
	if (mhash_get_keygen_max_key_size(KEYGEN_S2K_SALTED)==0) {
		keysize=100;
	} else {
		keysize = mhash_get_keygen_max_key_size(KEYGEN_S2K_SALTED);
	}
	if (mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED)==0) {
		salt_size=10;
	} else {
		salt_size = mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED);
	}
	
	salt = calloc( 1, salt_size);
	key = malloc( keysize);
	tmp = malloc( 2*keysize+1);
	
	data.hash_algorithm[0] = MHASH_SHA1;
	data.count = 0;
	data.salt = salt;
	data.salt_size = salt_size;
	
	mhash_keygen_ext(KEYGEN_S2K_SALTED, data, key, keysize, password, passlen);

	memset( tmp, 0, keysize*2);
	for (j = 0; j < keysize; j++) {
		sprintf(tmp2, "%.2x", key[j]);
		strcat(tmp, tmp2);
	}

	if (strcmp(KEY2, tmp)!=0) {
		fprintf(stderr, "KEYGEN-Test (KEYGEN_S2K_SALTED): Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", KEY2, tmp);
		free(password);
		free(tmp);
		free(key);
		return 1;
	}

	free(password);
	free(key);
	free(tmp);


	fprintf(stderr, "KEYGEN-Test: Succeed\n");

	return 0;
}
