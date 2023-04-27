#ifndef __COMMON_H_INCLUDE__
#define __COMMON_H_INCLUDE__

#define SZ_256			(1 << 8)
#define SZ_4K			(1 << 12)
#define SZ_8K			(1 << 13)
#define SZ_16K			(1 << 14)
#define SZ_64K			(1 << 16)
#define SZ_1M			(1 << 20)
#define SZ_2M			(1 << 21)
#define SZ_512M			(1 << 29)
#define SZ_1G			(1 << 30)
#define SZ_2G			(1ul << 31)
#define ARRAY_SIZE(_a) (sizeof(_a)/sizeof((_a)[0]))

#endif
