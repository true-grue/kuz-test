#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include "kuz_data.c"

#define ROUNDS 10

uint8_t in[16] = {
  0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88
};

uint8_t out[16];

const uint8_t key[] = {
  0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
  0xdb, 0x31, 0x48, 0x53, 0x15, 0x69, 0x43, 0x43, 0x22, 0x8d, 0x6a, 0xef, 0x8c, 0xc7, 0x8c, 0x44,
  0x3d, 0x45, 0x53, 0xd8, 0xe9, 0xcf, 0xec, 0x68, 0x15, 0xeb, 0xad, 0xc4, 0x0a, 0x9f, 0xfd, 0x04,
  0x57, 0x64, 0x64, 0x68, 0xc4, 0x4a, 0x5e, 0x28, 0xd3, 0xe5, 0x92, 0x46, 0xf4, 0x29, 0xf1, 0xac,
  0xbd, 0x07, 0x94, 0x35, 0x16, 0x5c, 0x64, 0x32, 0xb5, 0x32, 0xe8, 0x28, 0x34, 0xda, 0x58, 0x1b,
  0x51, 0xe6, 0x40, 0x75, 0x7e, 0x87, 0x45, 0xde, 0x70, 0x57, 0x27, 0x26, 0x5a, 0x00, 0x98, 0xb1,
  0x5a, 0x79, 0x25, 0x01, 0x7b, 0x9f, 0xdd, 0x3e, 0xd7, 0x2a, 0x91, 0xa2, 0x22, 0x86, 0xf9, 0x84,
  0xbb, 0x44, 0xe2, 0x53, 0x78, 0xc7, 0x31, 0x23, 0xa5, 0xf3, 0x2f, 0x73, 0xcd, 0xb6, 0xe5, 0x17,
  0x72, 0xe9, 0xdd, 0x74, 0x16, 0xbc, 0xf4, 0x5b, 0x75, 0x5d, 0xba, 0xa8, 0x8e, 0x4a, 0x40, 0x43,
};

#define GETADDR(table, i) ((uint64_t *) &table[((uint32_t) i) * 16])

#define R1(n, k) \
c0 ^= GETADDR(kuz_table, GETBYTE(b0, n) + k)[0]; \
c1 ^= GETADDR(kuz_table, GETBYTE(b0, n) + k)[1];

#define R2(n, k) \
c0 ^= GETADDR(kuz_table, GETBYTE(b1, n) + k)[0]; \
c1 ^= GETADDR(kuz_table, GETBYTE(b1, n) + k)[1];

#define GETBYTE(x, n) (((x) >> (8*n)) & 0xff)

#define ROUND(i) \
c0 = GETADDR(kuz_table, GETBYTE(b0, 0))[0]; \
c1 = GETADDR(kuz_table, GETBYTE(b0, 0))[1]; \
R1(1, 0x100) \
R1(2, 0x200) \
R1(3, 0x300) \
R1(4, 0x400) \
R1(5, 0x500) \
R1(6, 0x600) \
R1(7, 0x700) \
R2(0, 0x800) \
R2(1, 0x900) \
R2(2, 0xa00) \
R2(3, 0xb00) \
R2(4, 0xc00) \
R2(5, 0xd00) \
R2(6, 0xe00) \
R2(7, 0xf00) \
b0 = ((uint64_t *) key)[i * 2 + 0] ^ c0; \
b1 = ((uint64_t *) key)[i * 2 + 1] ^ c1;

void kuz_encrypt(const uint8_t *in, uint8_t *out, const uint8_t *key) {
  uint64_t c0, c1;
  uint64_t b0 = ((uint64_t *) in)[0] ^ ((uint64_t *) key)[0];
  uint64_t b1 = ((uint64_t *) in)[1] ^ ((uint64_t *) key)[1];
  ROUND(1)
  ROUND(2)
  ROUND(3)
  ROUND(4)
  ROUND(5)
  ROUND(6)
  ROUND(7)
  ROUND(8)
  ROUND(9)
  ((uint64_t *) out)[0] = b0;
  ((uint64_t *) out)[1] = b1;
}


void main() {
  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  for (int i = 0; i < 1000000; i += 1) {
    kuz_encrypt(in, out, key);
  }
  gettimeofday(&tv2, NULL);
  double t = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
    (double) (tv2.tv_sec - tv1.tv_sec);
  printf("%.2f MB/s\n", 16 / t);
  for (int i = 0; i < 16; i += 1) {
    printf("%02x", out[i]);
  }
  printf("\n");
}
