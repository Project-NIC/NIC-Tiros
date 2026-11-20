# NIC-Tiros — the plain-C core built and tested on the host: make test
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2
SRC = common/crc32.c common/frames.c common/settings.c common/hamilton.c common/bootlog.c common/airtime.c common/lastfix.c common/pieces.c common/recfile.c common/seal.c

test: tests/test_host.c $(SRC)
	$(CC) $(CFLAGS) -o tests/test_host tests/test_host.c tests/ccm_openssl.c $(SRC) -lm -lcrypto
	./tests/test_host

clean:
	rm -f tests/test_host
