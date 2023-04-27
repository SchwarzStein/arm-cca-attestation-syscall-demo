#include <stdio.h>
#include <qcbor/qcbor_decode.h>
#include <qcbor/qcbor_spiffy_decode.h>
#include <qcbor/UsefulBuf.h>
#include <stdlib.h>
#include "parser.h"

#define TOKEN_LEN  1218


#define TAG_COSE_SIGN1                       (18)
#define TAG_CCA_TOKEN                       (399)

#define CCA_PLAT_TOKEN                    (44234)    /* 0xACCA */
#define CCA_REALM_DELEGATED_TOKEN         (44241)

/* CCA Platform Attestation Token */
#define CCA_PLAT_CHALLENGE                   (10)    /* EAT nonce */
#define CCA_PLAT_INSTANCE_ID                (256)    /* EAT ueid */
#define CCA_PLAT_PROFILE                    (265)    /* EAT profile */
#define CCA_PLAT_SECURITY_LIFECYCLE        (2395)
#define CCA_PLAT_IMPLEMENTATION_ID         (2396)
#define CCA_PLAT_SW_COMPONENTS             (2399)
#define CCA_PLAT_VERIFICATION_SERVICE      (2400)
#define CCA_PLAT_CONFIGURATION             (2401)
#define CCA_PLAT_HASH_ALGO_ID              (2402)

/* CCA Realm Delegated Attestation Token */
#define CCA_REALM_CHALLENGE                  (10)    /* EAT nonce */ //BSTR
#define CCA_REALM_PERSONALIZATION_VALUE   (44235)  // BSTR
#define CCA_REALM_HASH_ALGO_ID            (44236)  // TEXT
#define CCA_REALM_PUB_KEY                 (44237)  // BSTR
#define CCA_REALM_INITIAL_MEASUREMENT     (44238)  // BSTR
#define CCA_REALM_EXTENSIBLE_MEASUREMENTS (44239)  // nx BSTR
#define CCA_REALM_PUB_KEY_HASH_ALGO_ID    (44240)  // TEXT

/* Software components */
#define CCA_SW_COMP_MEASUREMENT_VALUE         (2)
#define CCA_SW_COMP_VERSION                   (4)
#define CCA_SW_COMP_SIGNER_ID                 (5)
#define CCA_SW_COMP_HASH_ALGORITHM            (6)

#define RETURN_ON_DECODE_ERROR(p_context) \
	do { \
		QCBORError ret; \
		ret = QCBORDecode_GetError(p_context); \
		if (ret != QCBOR_SUCCESS) { \
			printf("QCBOR decode failed with error at %s:%d." \
				" err = %d\r\n", \
				__FILE__, (int)__LINE__, (int)ret); \
			return; \
		} \
	} while (0)

char token_buf[TOKEN_LEN];
char str_buf[TOKEN_LEN*2];

char hex_to_num(char ch){
	return (ch >= 'a')? (ch - 'a' + 10): (ch - '0');
}

char hex_to_byte(char s[]){
	return (hex_to_num(s[0])<<4) + hex_to_num(s[1]);
}

void print_buf(struct q_useful_buf_c b){
	int i;
	for (i=0;i<b.len;i++){
		printf("%02x",0x00ff&((char*)b.ptr)[i]);
	}
}

void test_parse(char token[], size_t token_size){
	struct q_useful_buf_c token_buf_qcbor = {token, token_size};

	struct q_useful_buf_c realm_token;
	struct q_useful_buf_c realm_token_payload;
	struct q_useful_buf_c realm_header;
	struct q_useful_buf_c realm_signature;

	struct q_useful_buf_c realm_pl_challenge;
	struct q_useful_buf_c realm_pl_personalize;
	struct q_useful_buf_c realm_pl_hash_algo;
	struct q_useful_buf_c realm_pl_pub_key;
	struct q_useful_buf_c realm_pl_init_measure;
	struct q_useful_buf_c realm_pl_ext_measure;
	struct q_useful_buf_c realm_pl_key_hash_algo;



	struct q_useful_buf_c platform_token;
	struct q_useful_buf_c platform_token_payload;

	// extract the 2 top level parts into sub-tokens
	// thanks kvm-unit-tests
	QCBORDecodeContext context;
	QCBORItem item;
	QCBORError err;
	QCBORDecode_Init(&context, token_buf_qcbor, QCBOR_DECODE_MODE_NORMAL);
	RETURN_ON_DECODE_ERROR(&context);
	// ERROR HANDLING
	QCBORDecode_EnterMap(&context, NULL);
	RETURN_ON_DECODE_ERROR(&context);
	QCBORDecode_GetByteStringInMapN(&context, CCA_PLAT_TOKEN,&platform_token);
	RETURN_ON_DECODE_ERROR(&context);
	QCBORDecode_GetByteStringInMapN(&context, CCA_REALM_DELEGATED_TOKEN,&realm_token);
	RETURN_ON_DECODE_ERROR(&context);
	QCBORDecode_ExitMap(&context);
	RETURN_ON_DECODE_ERROR(&context);



	// realm token wrapper COSE_Sign1 wrapper
	QCBORDecode_Init(&context, realm_token, QCBOR_DECODE_MODE_NORMAL);
	RETURN_ON_DECODE_ERROR(&context);
	
	// /* Check COSE tag. */
	// // QCBORDecode_PeekNext(&context, &item);
	// // if (!QCBORDecode_IsTagged(&context, &item,
	// // 	TAG_COSE_SIGN1)) {
	// // 	printf("wtf\n");
	// // 	return;
	// // }

	QCBORDecode_EnterArray(&context, NULL);

	/* Protected header */
	QCBORDecode_GetByteString(&context, &realm_header);

	QCBORDecode_EnterMap(&context, NULL);
	// handle the payload, skip for now
	QCBORDecode_ExitMap(&context);


	/* Payload */
	QCBORDecode_GetByteString(&context, &realm_token_payload);

	/* Signature */
	QCBORDecode_GetByteString(&context, &realm_signature);

	// leave current level
	QCBORDecode_ExitArray(&context);

	// look at the payload
	//
	
	QCBORDecode_Init(&context, realm_token_payload, QCBOR_DECODE_MODE_NORMAL);
	QCBORDecode_EnterMap(&context, NULL);
	// geather all the realm pl parts
	QCBORDecode_GetByteStringInMapN(&context, CCA_REALM_CHALLENGE ,&realm_pl_challenge);
	QCBORDecode_GetByteStringInMapN(&context, CCA_REALM_PERSONALIZATION_VALUE ,&realm_pl_personalize);
	QCBORDecode_GetTextStringInMapN(&context, CCA_REALM_HASH_ALGO_ID ,&realm_pl_hash_algo);
	QCBORDecode_GetByteStringInMapN(&context, CCA_REALM_PUB_KEY ,&realm_pl_pub_key);
	QCBORDecode_GetByteStringInMapN(&context, CCA_REALM_INITIAL_MEASUREMENT ,&realm_pl_init_measure);
	QCBORDecode_GetTextStringInMapN(&context, CCA_REALM_PUB_KEY_HASH_ALGO_ID ,&realm_pl_key_hash_algo);
	// geather ext measurements
	// struct q_useful_buf_c realm_pl_ext_measure;
	
	// leave
	QCBORDecode_ExitArray(&context);
	QCBORDecode_ExitMap(&context);
	QCBORDecode_Finish(&context);

	printf("\n\nrealm token header ---- \n");
	print_buf(realm_header);
	printf("\n\nrealm token payload ---- \n");
	print_buf(realm_token_payload);
	printf("\n\nrealm token sig ---- \n");
	print_buf(realm_signature);

	// struct q_useful_buf_c realm_pl_challenge;
	printf("\n\nrealm token challenge ---- \n");
	print_buf(realm_pl_challenge);
	
	// struct q_useful_buf_c realm_pl_personalize;
	printf("\n\nrealm token personalization ---- \n");
	print_buf(realm_pl_personalize);

	// struct q_useful_buf_c realm_pl_hash_algo;
	printf("\n\nrealm token hash algo ---- \n");
	print_buf(realm_pl_hash_algo);

	// struct q_useful_buf_c realm_pl_key_hash_algo;
	printf("\n\nrealm token pubkey hash algo ---- \n");
	print_buf(realm_pl_key_hash_algo);

	printf("\n\nrealm token pubkey ---- \n");
	print_buf(realm_pl_pub_key);

	printf("\n\nrealm token init measurement ---- \n");
	print_buf(realm_pl_init_measure);


	// struct q_useful_buf_c realm_pl_pub_key;
	// struct q_useful_buf_c realm_pl_init_measure;
	// struct q_useful_buf_c realm_pl_ext_measure;

}


int test_main(){
	int i;
	FILE * fptr = fopen("./cbor_full.txt", "r");
	for (i=0;i<TOKEN_LEN*2;i++){
		str_buf[i] = fgetc(fptr);
	}

	char* ptr = str_buf;

	for (i=0;i<TOKEN_LEN;i++){
		token_buf[i] = hex_to_byte(ptr);
		ptr += 2;
	}
	fclose(fptr);

	// for (i=0;i<TOKEN_LEN;i++){
	// 	printf("%02x",token_buf[i] & 0xff);
	// }
	test_parse(token_buf, TOKEN_LEN);
	return 0;
}


