libmincrypt_src_file := \
    src/dsa_sig.c \
    src/p256.c \
    src/p256_ec.c \
    src/p256_ecdsa.c \
    src/rsa.c \
    src/sha.c \
    src/sha256.c


libmincrypt_src_obj := $(libmincrypt_src_files:.c=.o)

all: libmincrypt.a

%.o: %.c
	gcc -I. -I.. -Imincrypt -DANDROID -c $< -o $@


libmincrypt.a: $(libmincrypt_src_obj)
	ar rvs $@ $^

.PHONY: clean clobber
clean:
	rm -f $(libmincrypt_src_obj) libmincrypt.a
clobber: clean
