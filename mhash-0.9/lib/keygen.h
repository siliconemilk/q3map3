
int _mhash_gen_key_asis(void *keyword, int key_size,
			unsigned char *password, int plen);
int _mhash_gen_key_mcrypt(hashid algorithm, void *keyword, int key_size,
			  void *salt, int salt_size,
			  unsigned char *password, int plen);
int _mhash_gen_key_hex(void *keyword, int key_size,
		       unsigned char *password, int plen);
int _mhash_gen_key_s2k_simple(hashid algorithm, void *keyword,
			      int key_size, unsigned char *password,
			      int plen);
int _mhash_gen_key_s2k_salted(hashid algorithm, void *keyword,
			      int key_size, unsigned char *salt,
			      int salt_size, unsigned char *password,
			      int plen);
int _mhash_gen_key_s2k_isalted(hashid algorithm, unsigned long count,
			       void *keyword, int key_size,
			       unsigned char *salt, int salt_size,
			       unsigned char *password, int plen);
int _mhash_gen_key_pkdes(void *keyword, int key_size,
			 unsigned char *password, int plen);
int _mhash_gen_key_crypt(void *keyword, int key_size,
			 unsigned char *password, int plen, void *salt,
			 int salt_size);
int _mhash_gen_key_scrypt(void *keyword, int key_size,
			  unsigned char *password, int plen);
