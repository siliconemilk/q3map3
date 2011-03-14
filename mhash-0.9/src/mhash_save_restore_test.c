#include <stdio.h>
#include <stdlib.h>
#include "../lib/mhash.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(void) {

	int i, buf_len;
	MHASH td1, td2, td3;
	const unsigned char *buf = "This is a test buffer to test saving and restoring, see?";
	unsigned char *hash1, *hash2;
	hashid alg;
	char mem[1024];
	int mem_size = sizeof(mem);

	buf_len = strlen(buf);

	/* NOTE: For laziness sake, I just loop through the enum, skipping invalid integers.
	   If the enum should change, this loop will have to change! */
	for (alg = 0; alg <= mhash_count(); ++alg) {
		
		/* if algorithm does not exist */
		if (mhash_get_hash_name_static( alg)==NULL)
			continue;

		printf("Testing save/restore for algorithm %s: ", mhash_get_hash_name(alg));

		td1 = mhash_init(alg);

		if (td1 == MHASH_FAILED) {
			fprintf(stderr, "Failed to init td1.\n");
			exit(1);
		}

		for (i = 0; i < buf_len; ++i)
			mhash(td1, buf+i, 1);

		hash1 = mhash_end(td1);

/*		printf("Hash 1: ");
		for (i = 0; i < mhash_get_block_size(alg); ++i)
			printf("%.2x", hash1[i]);
		printf("\n");
*/
		td2 = mhash_init(alg);

		if (td2 == MHASH_FAILED) {
			fprintf(stderr, "Failed to init td2.\n");
			exit(1);
		}

		for (i = 0; i < buf_len/2; ++i)
			mhash(td2, buf+i, 1);

		if (mhash_save_state_mem(td2, mem, &mem_size)!=0) {
			fprintf(stderr, "Error saving state. Size: %d\n", mem_size);
			exit(1);
		}

		td3 = mhash_restore_state_mem( mem);

		if (td3 == MHASH_FAILED) {
			fprintf(stderr, "Error restoring state.\n");
			exit(1);
		}

		for (i = buf_len/2; i < buf_len; ++i)
			mhash(td3, buf+i, 1);

		hash2 = mhash_end(td3);

/*		printf("Hash 2: ");
		for (i = 0; i < mhash_get_block_size(alg); ++i)
			printf("%.2x", hash2[i]);
		printf("\n");
*/
		if (memcmp(hash1, hash2, mhash_get_block_size(alg)) == 0) {
			printf("Ok\n");
		} else {
			printf("Failed\n");
			exit(1);
		}
	}
	exit(0);
}

