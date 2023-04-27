#ifndef __ATTEST_USER_H
#define __ATTEST_USER_H

#include "common.h"
#include "unistd.h"
#include "stdio.h"
#define RSI_ATTEST_TOKEN_SYSCALL 452

struct smc_reuslts {
	unsigned long r[10];
};

// return length of token on success, otherwise return -1
int get_attest_token(char* token_buffer_4k, char* challenge);


#endif
