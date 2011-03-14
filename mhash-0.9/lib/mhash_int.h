
#include <libdefs.h>

char* mystrdup(char*);

enum hashid {
	MHASH_CRC32,
	MHASH_MD5,
	MHASH_SHA1,
	MHASH_HAVAL256,
	MHASH_RIPEMD160 = 5,
	MHASH_TIGER = 7,
	MHASH_GOST,
	MHASH_CRC32B,
	MHASH_HAVAL224=10,
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
	MHASH_WHIRLPOOL,
	MHASH_RIPEMD128,
	MHASH_RIPEMD256,
	MHASH_RIPEMD320,
};

enum keygenid {
	KEYGEN_MCRYPT,		/* The key generator used in mcrypt */
	KEYGEN_ASIS,		/* Just returns the password as binary key */
	KEYGEN_HEX,		/* Just converts a hex key into a binary one */
	KEYGEN_PKDES,		/* The transformation used in Phil Karn's DES
				 * encryption program */
	KEYGEN_S2K_SIMPLE,	/* The OpenPGP (rfc2440) Simple S2K */
	KEYGEN_S2K_SALTED,	/* The OpenPGP Salted S2K */
	KEYGEN_S2K_ISALTED	/* The OpenPGP Iterated Salted S2K */
};

typedef enum hashid hashid;
typedef enum keygenid keygenid;

typedef struct keygen {
	hashid 		hash_algorithm[2];
	unsigned int 	count;
	void*		salt;
	int		salt_size;
} KEYGEN;

typedef void (*INIT_FUNC)( void*);
typedef void (*HASH_FUNC)(void*, const void*, int);
typedef void (*FINAL_FUNC)(void*);
typedef void (*DEINIT_FUNC)(void*, unsigned char*);

typedef struct {
	int hmac_key_size;
	int hmac_block;
	unsigned char *hmac_key;

	word8 *state;
	int	state_size;
	hashid algorithm_given;

	HASH_FUNC hash_func;
	FINAL_FUNC final_func;
	DEINIT_FUNC deinit_func;
	                        
} MHASH_INSTANCE;

typedef MHASH_INSTANCE * MHASH;


typedef struct mhash_hash_entry mhash_hash_entry;

#define MHASH_FAILED ((MHASH) 0x0)

/* information prototypes */

WIN32DLL_DEFINE size_t mhash_count(void);
WIN32DLL_DEFINE size_t mhash_get_block_size(hashid type);
WIN32DLL_DEFINE char *mhash_get_hash_name(hashid type);

/* initializing prototypes */

WIN32DLL_DEFINE MHASH mhash_init(hashid type);
WIN32DLL_DEFINE MHASH mhash_init_int(hashid type);

/* update prototype */

WIN32DLL_DEFINE int mhash(MHASH thread, const void *plaintext, size_t size);

/* finalizing prototype */

WIN32DLL_DEFINE void *mhash_end(MHASH thread);

WIN32DLL_DEFINE size_t mhash_get_hash_pblock(hashid type);
WIN32DLL_DEFINE MHASH hmac_mhash_init(const hashid type, void *key, int keysize,
		      int block);
WIN32DLL_DEFINE void *hmac_mhash_end(MHASH thread);

/* Key generation functions */
WIN32DLL_DEFINE int mhash_keygen(keygenid algorithm, hashid opt_algorithm,
		 unsigned long count, void *keyword, int keysize,
		 void *salt, int saltsize, unsigned char *password,
		 int passwordlen);
WIN32DLL_DEFINE int mhash_keygen_ext(keygenid algorithm, KEYGEN data,
	 void *keyword, int keysize,
	 unsigned char *password, int passwordlen);

WIN32DLL_DEFINE char *mhash_get_keygen_name(hashid type);
WIN32DLL_DEFINE size_t mhash_get_keygen_salt_size(keygenid type);
WIN32DLL_DEFINE size_t mhash_keygen_count(void);
WIN32DLL_DEFINE int mhash_keygen_uses_salt(keygenid type);
WIN32DLL_DEFINE int mhash_keygen_uses_count(keygenid type);
WIN32DLL_DEFINE int mhash_keygen_uses_hash_algorithm(keygenid type);
