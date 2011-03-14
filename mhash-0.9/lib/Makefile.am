#AUTOMAKE_OPTIONS = foreign

noinst_HEADERS = mhash_ripemd.h mhash_gost.h mhash_crc32.h mhash_md5.h \
	mhash_tiger.h libdefs.h mhash_haval.h mhash_sha1.h keygen.h \
	mhash_int.h mhash_md4.h mhash_sha256_sha224.h mhash_sha512_sha384.h \
	mhash_adler32.h mhash_whirlpool.h
 
include_HEADERS = mhash.h

lib_LTLIBRARIES = libmhash.la

libmhash_la_SOURCES = mhash.c swap.c bzero.c crc32.c md5.c sha1.c haval.c \
		tiger.c tiger_sboxes.c gosthash.c \
		keygen_hex.c keygen_mcrypt.c keygen_asis.c keygen.c \
		keygen_s2k.c ripemd.c md4.c sha256_sha224.c sha512_sha384.c \
		adler32.c whirlpool.c

libmhash_la_LDFLAGS = -version-info 2 -no-undefined -export-symbols $(srcdir)/libmhash.sym

EXTRA_DIST = libmhash.sym

dist-hook:
	find $(distdir) -name CVS -o -name .cvsignore | xargs rm -rf
