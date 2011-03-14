/*
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


/* According to Tiger's web page 
 * http://www.cs.technion.ac.il/~biham/Reports/Tiger/
 *
 * Tiger has no usage restrictions nor patents. It can be used freely, with the
 * reference implementation, with other implementations or with a modification 
 * to the reference implementation (as long as it still implements Tiger).
 *
 * This implementation is based on the reference implementation.
 *
 *							--nikos
 */

#include "libdefs.h"

#ifdef ENABLE_TIGER

#include "mhash_tiger.h"

#define PASSES 3

extern const word32 tiger_table[4*256][2];

#define t1 (tiger_table)
#define t2 (tiger_table+256)
#define t3 (tiger_table+256*2)
#define t4 (tiger_table+256*3)

#define sub64(s0, s1, p0, p1) \
      temps0 = (p0); \
      tcarry = s0 < temps0; \
      s0 -= temps0; \
      s1 -= (p1) + tcarry;

#define add64(s0, s1, p0, p1) \
      temps0 = (p0); \
      s0 += temps0; \
      tcarry = s0 < temps0; \
      s1 += (p1) + tcarry;

#define xor64(s0, s1, p0, p1) \
      s0 ^= (p0); \
      s1 ^= (p1);

#define mul5(s0, s1) \
      tempt0 = s0<<2; \
      tempt1 = (s1<<2)|(s0>>30); \
      add64(s0, s1, tempt0, tempt1);

#define mul7(s0, s1) \
      tempt0 = s0<<3; \
      tempt1 = (s1<<3)|(s0>>29); \
      sub64(tempt0, tempt1, s0, s1); \
      s0 = tempt0; \
      s1 = tempt1;

#define mul9(s0, s1) \
      tempt0 = s0<<3; \
      tempt1 = (s1<<3)|(s0>>29); \
      add64(s0, s1, tempt0, tempt1);

#define save_abc \
      aa0 = a0; \
      aa1 = a1; \
      bb0 = b0; \
      bb1 = b1; \
      cc0 = c0; \
      cc1 = c1;

#define round(a0,a1,b0,b1,c0,c1,x0,x1,mul) \
      xor64(c0, c1, x0, x1); \
      temp0  = t1[((c0)>>(0*8))&0xFF][0] ; \
      temp1  = t1[((c0)>>(0*8))&0xFF][1] ; \
      temp0 ^= t2[((c0)>>(2*8))&0xFF][0] ; \
      temp1 ^= t2[((c0)>>(2*8))&0xFF][1] ; \
      temp0 ^= t3[((c1)>>(0*8))&0xFF][0] ; \
      temp1 ^= t3[((c1)>>(0*8))&0xFF][1] ; \
      temp0 ^= t4[((c1)>>(2*8))&0xFF][0] ; \
      temp1 ^= t4[((c1)>>(2*8))&0xFF][1] ; \
      sub64(a0, a1, temp0, temp1); \
      temp0  = t4[((c0)>>(1*8))&0xFF][0] ; \
      temp1  = t4[((c0)>>(1*8))&0xFF][1] ; \
      temp0 ^= t3[((c0)>>(3*8))&0xFF][0] ; \
      temp1 ^= t3[((c0)>>(3*8))&0xFF][1] ; \
      temp0 ^= t2[((c1)>>(1*8))&0xFF][0] ; \
      temp1 ^= t2[((c1)>>(1*8))&0xFF][1] ; \
      temp0 ^= t1[((c1)>>(3*8))&0xFF][0] ; \
      temp1 ^= t1[((c1)>>(3*8))&0xFF][1] ; \
      add64(b0, b1, temp0, temp1); \
      if((mul)==5) \
	{mul5(b0, b1);} \
      else \
	if((mul)==7) \
	  {mul7(b0, b1);} \
	else \
	  {mul9(b0, b1)};

#define pass(a0,a1,b0,b1,c0,c1,mul) \
      round(a0,a1,b0,b1,c0,c1,x00,x01,mul); \
      round(b0,b1,c0,c1,a0,a1,x10,x11,mul); \
      round(c0,c1,a0,a1,b0,b1,x20,x21,mul); \
      round(a0,a1,b0,b1,c0,c1,x30,x31,mul); \
      round(b0,b1,c0,c1,a0,a1,x40,x41,mul); \
      round(c0,c1,a0,a1,b0,b1,x50,x51,mul); \
      round(a0,a1,b0,b1,c0,c1,x60,x61,mul); \
      round(b0,b1,c0,c1,a0,a1,x70,x71,mul);

#define key_schedule \
      sub64(x00, x01, x70^0xA5A5A5A5, x71^0xA5A5A5A5); \
      xor64(x10, x11, x00, x01); \
      add64(x20, x21, x10, x11); \
      sub64(x30, x31, x20^((~x10)<<19), ~x21^(((x11)<<19)|((x10)>>13))); \
      xor64(x40, x41, x30, x31); \
      add64(x50, x51, x40, x41); \
      sub64(x60, x61, ~x50^(((x40)>>23)|((x41)<<9)), x51^((~x41)>>23)); \
      xor64(x70, x71, x60, x61); \
      add64(x00, x01, x70, x71); \
      sub64(x10, x11, x00^((~x70)<<19), ~x01^(((x71)<<19)|((x70)>>13))); \
      xor64(x20, x21, x10, x11); \
      add64(x30, x31, x20, x21); \
      sub64(x40, x41, ~x30^(((x20)>>23)|((x21)<<9)), x31^((~x21)>>23)); \
      xor64(x50, x51, x40, x41); \
      add64(x60, x61, x50, x51); \
      sub64(x70, x71, x60^0x89ABCDEF, x61^0x01234567);

#define feedforward \
      xor64(a0, a1, aa0, aa1); \
      sub64(b0, b1, bb0, bb1); \
      add64(c0, c1, cc0, cc1);

#ifdef UNROLL_COMPRESS
#define compress \
      save_abc \
      pass(a0,a1,b0,b1,c0,c1,5); \
      key_schedule; \
      pass(c0,c1,a0,a1,b0,b1,7); \
      key_schedule; \
      pass(b0,b1,c0,c1,a0,a1,9); \
      for(pass_no=3; pass_no<PASSES; pass_no++) { \
        key_schedule \
	pass(a0,a1,b0,b1,c0,c1,9); \
	tmpa=a0; a0=c0; c0=b0; b0=tmpa; \
	tmpa=a1; a1=c1; c1=b1; b1=tmpa;} \
      feedforward
#else
#define compress \
      save_abc \
      for(pass_no=0; pass_no<PASSES; pass_no++) { \
        if(pass_no != 0) {key_schedule} \
	pass(a0,a1,b0,b1,c0,c1,(pass_no==0?5:pass_no==1?7:9)) \
	tmpa=a0; a0=c0; c0=b0; b0=tmpa; \
	tmpa=a1; a1=c1; c1=b1; b1=tmpa;} \
      feedforward
#endif

#define tiger_compress_macro(str, state) \
{ \
  register word32 a0, a1, b0, b1, c0, c1, tmpa; \
  word32 aa0, aa1, bb0, bb1, cc0, cc1; \
  word32 x00, x01, x10, x11, x20, x21, x30, x31, \
         x40, x41, x50, x51, x60, x61, x70, x71; \
  register word32 temp0, temp1, tempt0, tempt1, temps0, tcarry; \
  word32 i; \
  int pass_no; \
\
  a0 = state[0]; \
  a1 = state[1]; \
  b0 = state[2]; \
  b1 = state[3]; \
  c0 = state[4]; \
  c1 = state[5]; \
\
  x00=str[0*2]; x01=str[0*2+1]; x10=str[1*2]; x11=str[1*2+1]; \
  x20=str[2*2]; x21=str[2*2+1]; x30=str[3*2]; x31=str[3*2+1]; \
  x40=str[4*2]; x41=str[4*2+1]; x50=str[5*2]; x51=str[5*2+1]; \
  x60=str[6*2]; x61=str[6*2+1]; x70=str[7*2]; x71=str[7*2+1]; \
\
  compress; \
\
  state[0] = a0; \
  state[1] = a1; \
  state[2] = b0; \
  state[3] = b1; \
  state[4] = c0; \
  state[5] = c1; \
}

#define UNROLL_COMPRESS
#ifdef UNROLL_COMPRESS
/* The compress function is inlined */
#define tiger_compress(str, state) \
  tiger_compress_macro(((word32*)str), ((word32*)state))
#else
/* The compress function is a function */
tiger_compress(const word32 *str, word32 state[6])
{
  tiger_compress_macro(((word32*)str), ((word32*)state));
}
#endif


#ifndef EXTRACT_UCHAR
#define EXTRACT_UCHAR(p)  (*(word8 *)(p))
#endif

#define STRING2INT(s) ((((((EXTRACT_UCHAR(s+3) << 8)    \
			 | EXTRACT_UCHAR(s+2)) << 8)  \
			 | EXTRACT_UCHAR(s+1)) << 8)  \
			 | EXTRACT_UCHAR(s))

static void tiger_block(struct tiger_ctx *ctx, word8 * block)
{
	word32 data[TIGER_DATALEN];
	int i;
	
	/* Update block count */
	if (!++ctx->count_l)
		++ctx->count_h;

	/* Endian independent conversion */
	for (i = 0; i < TIGER_DATALEN; i++, block += sizeof(word32))
		data[i] = STRING2INT(block);

     tiger_compress(data, ctx->digest);
}

void tiger_init(struct tiger_ctx *ctx)
{
	/* Set the h-vars to their initial values */
	ctx->digest[0] = h0init;
	ctx->digest[1] = h1init;
	ctx->digest[2] = h2init;
	ctx->digest[3] = h3init;
	ctx->digest[4] = h4init;
	ctx->digest[5] = h5init;

	/* Initialize bit count */
	ctx->count_l = ctx->count_h = 0;

	/* Initialize buffer */
	ctx->index = 0;
}

void tiger_update(struct tiger_ctx *ctx, word8 * buffer, word32 len)
{
	if (ctx->index) {	/* Try to fill partial block */
		unsigned left = TIGER_DATASIZE - ctx->index;
		if (len < left) {
			memcpy(ctx->block + ctx->index, buffer, len);
			ctx->index += len;
			return;	/* Finished */
		} else {
			memcpy(ctx->block + ctx->index, buffer, left);
		     tiger_block(ctx, ctx->block);
			buffer += left;
			len -= left;
		}
	}
	while (len >= TIGER_DATASIZE) {
	     tiger_block(ctx, buffer);
		buffer += TIGER_DATASIZE;
		len -= TIGER_DATASIZE;
	}
	if ((ctx->index = len))
		/* This assignment is intended */
		/* Buffer leftovers */
		memcpy(ctx->block, buffer, len);
}

void tiger_final(struct tiger_ctx *ctx)
{
	word32 data[TIGER_DATALEN];
	int i;
	int words;

	i = ctx->index;
	/* Set the first char of padding to 0x01.  This is safe since there is
	   always at least one byte free */
	ctx->block[i++] = 0x01;

	/* Fill rest of word */
	for (; i & 3; i++)
		ctx->block[i] = 0;

	/* i is now a multiple of the word size 4 */
	words = i >> 2;
	for (i = 0; i < words; i++)
		data[i] = STRING2INT(ctx->block + 4 * i);

	if (words > (TIGER_DATALEN - 2)) {	/* No room for length in this block. Process it and
						 * pad with another one */
		for (i = words; i < TIGER_DATALEN; i++)
			data[i] = 0;
		tiger_compress( data, ctx->digest);
		for (i = 0; i < (TIGER_DATALEN - 2); i++)
			data[i] = 0;
	} else
		for (i = words; i < TIGER_DATALEN - 2; i++)
			data[i] = 0;
	/* Theres 512 = 2^9 bits in one block */
	data[TIGER_DATALEN - 1] =
	    (ctx->count_h << 9) | (ctx->count_l >> 23);
	data[TIGER_DATALEN - 2] = (ctx->count_l << 9) | (ctx->index << 3);
	tiger_compress( data, ctx->digest);
}

void tiger_digest(struct tiger_ctx *ctx, word8 * s)
{
	int i;

	if (s!=NULL)
	for (i = 0; i < TIGER_DIGESTLEN; i+=2) { /* 64 bit and LITTLE ENDIAN -that's cool! */
		s[7] = ctx->digest[i];
		s[6] = 0xff & (ctx->digest[i] >> 8);
		s[5] = 0xff & (ctx->digest[i] >> 16);
		s[4] = 0xff & ctx->digest[i] >> 24;
		s[3] = ctx->digest[i+1];
		s[2] = 0xff & (ctx->digest[i+1] >> 8);
		s[1] = 0xff & (ctx->digest[i+1] >> 16);
		s[0] = 0xff & ctx->digest[i+1] >> 24;
		s+=8;
	}

}

void tiger128_digest(struct tiger_ctx *ctx, word8 * s)
{
	int i;

	if (s!=NULL)
	for (i = 0; i < TIGER128_DIGESTLEN; i+=2) { /* 64 bit and LITTLE ENDIAN -that's cool! */
		s[7] = ctx->digest[i];
		s[6] = 0xff & (ctx->digest[i] >> 8);
		s[5] = 0xff & (ctx->digest[i] >> 16);
		s[4] = 0xff & ctx->digest[i] >> 24;
		s[3] = ctx->digest[i+1];
		s[2] = 0xff & (ctx->digest[i+1] >> 8);
		s[1] = 0xff & (ctx->digest[i+1] >> 16);
		s[0] = 0xff & ctx->digest[i+1] >> 24;
		s+=8;
	}

}


void tiger160_digest(struct tiger_ctx *ctx, word8 * s)
{
	int i;

	for (i = 0; i < TIGER160_DIGESTLEN-1; i+=2) { /* 64 bit and LITTLE ENDIAN -that's cool! */
		s[7] = ctx->digest[i];
		s[6] = 0xff & (ctx->digest[i] >> 8);
		s[5] = 0xff & (ctx->digest[i] >> 16);
		s[4] = 0xff & ctx->digest[i] >> 24;
		s[3] = ctx->digest[i+1];
		s[2] = 0xff & (ctx->digest[i+1] >> 8);
		s[1] = 0xff & (ctx->digest[i+1] >> 16);
		s[0] = 0xff & ctx->digest[i+1] >> 24;
		s+=8;
	}
	/* last block */
	s[3] = ctx->digest[i+1];
	s[2] = 0xff & (ctx->digest[i+1] >> 8);
	s[1] = 0xff & (ctx->digest[i+1] >> 16);
	s[0] = 0xff & ctx->digest[i+1] >> 24;
}

#endif /* ENABLE_TIGER */
