/* NIC-Tiros — CRC-32, IEEE 802.3, as zlib's crc32(): the record's blocks, the tables, the boot log. */
#ifndef TIROS_CRC32_H
#define TIROS_CRC32_H
#include <stddef.h>
#include <stdint.h>

/* Continue a CRC: start with crc = 0, as zlib's crc32(0, …). */
uint32_t crc32_update(uint32_t crc, const void *data, size_t len);

#endif
