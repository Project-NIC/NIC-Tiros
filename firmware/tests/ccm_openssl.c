/* NIC-Tiros — AES-128-CCM on the host for the tests, through OpenSSL: seal.h's ccm_fn. */
#include <openssl/evp.h>
#include "../common/seal.h"

int ccm_openssl(const uint8_t key[16], const uint8_t nonce[NONCE_LEN], const uint8_t *aad, size_t aad_len,
		const uint8_t *in, uint8_t *out, size_t len, uint8_t tag[4], int encrypt)
{
	EVP_CIPHER_CTX *c = EVP_CIPHER_CTX_new();
	int n, ok = 0;

	if (!c) {
		return -1;
	}
	if (EVP_CipherInit_ex(c, EVP_aes_128_ccm(), 0, 0, 0, encrypt) == 1 &&
	    EVP_CIPHER_CTX_ctrl(c, EVP_CTRL_CCM_SET_IVLEN, NONCE_LEN, 0) == 1 &&
	    EVP_CIPHER_CTX_ctrl(c, EVP_CTRL_CCM_SET_TAG, 4, encrypt ? 0 : tag) == 1 &&
	    EVP_CipherInit_ex(c, 0, 0, key, nonce, encrypt) == 1 &&
	    EVP_CipherUpdate(c, 0, &n, 0, (int)len) == 1 &&
	    (!aad_len || EVP_CipherUpdate(c, 0, &n, aad, (int)aad_len) == 1)) {
		uint8_t dummy;
		ok = EVP_CipherUpdate(c, len ? out : &dummy, &n, len ? in : &dummy, (int)len) == 1;
		if (ok && encrypt) {
			ok = EVP_CipherFinal_ex(c, out ? out : &dummy, &n) == 1 &&
			     EVP_CIPHER_CTX_ctrl(c, EVP_CTRL_CCM_GET_TAG, 4, tag) == 1;
		}
	}
	EVP_CIPHER_CTX_free(c);
	return ok ? 0 : -1;
}
