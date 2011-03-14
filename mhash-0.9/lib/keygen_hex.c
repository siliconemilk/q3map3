/*
 *    Copyright (C) 1998,2002 Nikos Mavroyanopoulos
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



#include "libdefs.h"

#ifdef WIN32
/* Provided by Stefan Hetzl <shetzl@chello.at>
 * in order for isxdigit to work on cygwin.
 */
static char hexdigits[] = { '0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9',
			'a', 'b', 'c', 'd', 'e', 'f',
			'A', 'B', 'C', 'D', 'E', 'F' } ;

static int ishexdigit (char d)
{
	int i;

	for (i = 0 ; i < 22 ; i++)
		if (d == hexdigits[i]) 
			return -1;

	return 0 ;
}
#else
# include <ctype.h>
# define ishexdigit isxdigit
#endif

static int check_hex(char *given_chain, int len)
{
	int i;

	for (i = 0; i < len; i++)
		if (ishexdigit(given_chain[i]) == 0)
			return -1;

	return 0;
}


int _mhash_gen_key_hex(void *keyword, int key_size, 
		unsigned char *password, int plen)
{
	char *chain = (char*) password;
	char* pkeyword=keyword;
	char tmp[3];
	int i;

	mhash_bzero( keyword, key_size);
	/* The chain should have 2*n characters 
	 */
	if (plen % 2 != 0 || plen > key_size*2)
		return -1;
	if (check_hex(chain, plen) == -1)
		return -1;

	memset( keyword, 0, key_size);
	for (i = 0; i < plen; i += 2) {
		memcpy(tmp, &chain[i], 2);
		tmp[2]='\0';
		pkeyword[i / 2] = strtol(tmp, (char **) NULL, 16);
	}

	return 0;
}


