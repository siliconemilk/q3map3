/* mhash_whirlpool.h
 * 
 * The Whirlpool hashing function. A new 512-bit hashing function
 * operating on messages less than 2 ** 256 bits in length. The
 * function structure is designed according to the Wide Trail strategy
 * and permits a wide variety of implementation trade-offs.
 *
 * The following code was written by B. Poettering for the mhash library.
 * The code is in the public domain. See whirlpool.c for more information.
 *
 */

#ifndef MHASH_WHIRLPOOL_H_INCLUDED
#define MHASH_WHIRLPOOL_H_INCLUDED


#define WHIRLPOOL_DIGEST_SIZE 64
#define WHIRLPOOL_DATA_SIZE 64


typedef struct whirlpool_ctx
{
  byte buffer[WHIRLPOOL_DATA_SIZE];	   /* buffer of data to hash */
  word64 hashlen[4];                       /* number of hashed bits (256-bit) */
  int index;		                   /* index to buffer */
  word64 hash[WHIRLPOOL_DIGEST_SIZE/8];    /* the hashing state */
} WHIRLPOOL_CTX;

void
whirlpool_init(struct whirlpool_ctx *ctx);

void
whirlpool_update(struct whirlpool_ctx *ctx, const byte *data, unsigned length);

void
whirlpool_final(struct whirlpool_ctx *ctx);

void
whirlpool_digest(const struct whirlpool_ctx *ctx, byte *digest);


#endif   /* MHASH_WHIRLPOOL_H_INCLUDED */
