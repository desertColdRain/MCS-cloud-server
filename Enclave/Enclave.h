#ifndef ENCLAVE_H
#define ENCLAVE_H

#include "pbc/pbc.h"
//#include "openssl/sha.h"
#define AES_TAG_SIZE          16
#define AES_IV_SIZE        12
#define PROCESS_LEVEL        4
#define MAX_GET_TOKENI_NUM 10
size_t ID_nonfirst[MAX_GET_TOKENI_NUM+1];
pairing_t pairing;
element_t g_g;
element_t g, h;
element_t public_key, secret_key;
element_t sig;
element_t tmp1,tmp2,tmp3;
element_t temp1, temp2;
element_t test_sk;
element_t hash;
//received from client public key and signature
element_t public;
element_t signature;

//sgx_status_t e_pairing_init(char* param, size_t count);
//sgx_status_t verifySignature(char* msg, element_t pub_key,element_t signature);
#endif

