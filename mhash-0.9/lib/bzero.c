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


/*
   $Id: bzero.c,v 1.1.1.1 2000/04/04 10:34:24 nmav Exp $ 
 */

#include "libdefs.h"

void
mhash_bzero(void *s, int n)
{
#ifdef HAVE_MEMSET
	memset(s, '\0', n);
#else
#ifdef HAVE_BZERO
	bzero(s, n);
#else
	char *stmp = s;

	for (int i = 0; i < n; i++)
		stmp[i] = '\0';

#endif
#endif
}
