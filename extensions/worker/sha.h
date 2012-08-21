/* 
 * $Id: sha.h,v 1.1 2002/01/28 11:03:44 mishan Exp $
 */

#include <stdint.h>
#include <stddef.h>

/* The SHA block size and message digest sizes, in bytes */

#define SHA_DATASIZE    64
#define SHA_DATALEN     16
#define SHA_DIGESTSIZE  20
#define SHA_DIGESTLEN    5
/* The structure for storing SHA info */

struct sha_ctx {
  uint32_t digest[SHA_DIGESTLEN];  /* Message digest */
  uint32_t count_l, count_h;       /* 64-bit block count */
  uint8_t block[SHA_DATASIZE];     /* SHA data buffer */
  int index;                        /* index into buffer */
};

extern void sha_init(struct sha_ctx *ctx);
extern void sha_update(struct sha_ctx *ctx, uint8_t *buffer, uint32_t len);
extern void sha_final(uint8_t *s, struct sha_ctx *ctx);
extern void sha_digest(struct sha_ctx *ctx, uint8_t *s);
extern void sha_copy(struct sha_ctx *dest, struct sha_ctx *src);
extern int sha_fd(int fd, size_t maxlen, uint8_t *s);
extern void sha1(char *input, char *digest);