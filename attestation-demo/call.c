// part of the code is copied / modified from kvm-unit-tests
#include<unistd.h>
#include "common.h"
#include "call.h"
// #include <qcbor/qcbor_decode.h>
// #include <qcbor/qcbor_spiffy_decode.h>
// #include <qcbor/UsefulBuf.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

struct smc_reuslts {
	unsigned long r[10];
};

int main()
{
	char* token_buffer = (char*)malloc(SZ_4K);
	int i;
	int ret;
	unsigned long* challenge = (unsigned long*) malloc(8*sizeof(unsigned long));
	struct smc_reuslts res;
	memset(challenge, 0x42, 8*sizeof(unsigned long));

    // syscall(451);
    syscall(452, token_buffer, SZ_4K, challenge, (void*)&res);
	
	
	
	/* Returns TOKEN_VERIFICATION_ERR* */
	if (res.r[0] != 0) {
		// fails 
		printf("smc call exits with return code %lu\n", res.r[0]);
		return 0;
	}

	printf("USERSPACE ATTEST TOKEN:\n");
	for (i=0;i<200;i=i+2){
		printf("%4d %lx %lx\n",i, ((unsigned long*)token_buffer)[i],((unsigned long*)token_buffer)[i+1]);
	}

	printf("smc call success, attest token size is %lu\n", res.r[1]);
	printf("token verifier returns with code %d\n", ret);
	// printf("START OF RAW TOKEN -- \n");
	// print_raw_token(token_buffer, res.r[1]);
	// printf("END OF RAW TOKEN --\n");
	printf("realm token printed\n");
	test_parse(token_buffer, res.r[1]);


	
    return 0;
}


