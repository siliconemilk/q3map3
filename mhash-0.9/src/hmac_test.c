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

#define KEY1 "Jefe"
#define DATA1 "what do ya want for nothing?"
#define DIGEST1 "750c783e6ab0b503eaa86e310a5db738"

#define KEY2 ""
#define DATA2 "Hi There"
#define DIGEST2 "9294727a3638bb1c13f48ef8158bfc9d"

int main()
{

	char tmp[128];
	char tmp2[3];
	char *password;
	int passlen;
	char *data;
	int datalen;
	MHASH td;
	unsigned char *mac;
	int j;

	memset(tmp, 0, sizeof(tmp));

	passlen=sizeof(KEY1) - 1;
	password = malloc(passlen+1);
	memcpy(password, KEY1, passlen);
	
	datalen=strlen(DATA1);
	data=malloc(datalen+1);
	strcpy(data, DATA1);

	td =
	    mhash_hmac_init(MHASH_MD5, password, passlen,
			    mhash_get_hash_pblock(MHASH_MD5));

	mhash(td, data, datalen);
	mac = mhash_hmac_end(td);

	tmp2[2]='\0';
	
	for (j = 0; j < mhash_get_block_size(MHASH_MD5); j++) {
		sprintf(tmp2, "%.2x", mac[j]);
		strcat(tmp, tmp2);
	}

	if (strcmp(DIGEST1, tmp)!=0) {
		fprintf(stderr, "HMAC-Test: Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", DIGEST1, tmp);
		free(password);
		free(data);
		return 1;
	}

		free(password);
		free(data);
	
	/* Test No 2 */	

	memset( tmp, 0, sizeof(tmp));
	
	passlen=sizeof(KEY2) - 1;
	password = malloc(passlen+1);
	memcpy(password, KEY2, passlen);
	
	datalen=strlen(DATA2);
	data=malloc(datalen+1);
	strcpy(data, DATA2);

	td =
	    mhash_hmac_init(MHASH_MD5, password, passlen,
			    mhash_get_hash_pblock(MHASH_MD5));

	mhash(td, data, datalen);
	mac = mhash_hmac_end(td);

	tmp2[2]='\0';
	
	for (j = 0; j < mhash_get_block_size(MHASH_MD5); j++) {
		sprintf(tmp2, "%.2x", mac[j]);
		strcat(tmp, tmp2);
	}

	if (strcmp(DIGEST2, tmp)!=0) {
		fprintf(stderr, "HMAC-Test: Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", DIGEST2, tmp);
		free(password);
		free(data);
		return 1;
	}


	free(password);
	free(data);

	fprintf(stderr, "MD5 HMAC-Test: Ok\n");

	return 0;
}
