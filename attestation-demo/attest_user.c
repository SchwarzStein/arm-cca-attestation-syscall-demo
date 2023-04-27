#include "attest_user.h"

int get_attest_token(char* token_buffer_4k, char* challenge)
{

    // syscall(451);
	struct smc_reuslts res;
    syscall(RSI_ATTEST_TOKEN_SYSCALL, token_buffer_4k, SZ_4K, challenge, (void*)&res);
	int i;
	/* Returns TOKEN_VERIFICATION_ERR* */
	if (res.r[0] != 0) {
		// fails 
		printf("smc call exits with return code %lu\n", res.r[0]);
		return -1;
	}
	printf("smc call success, attest token size is %lu\n", res.r[1]);
	return res.r[1];

}

