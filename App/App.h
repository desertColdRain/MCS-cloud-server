#ifndef _APP_H_
#define _APP_H_

#include <stdio.h>
#include <stdlib.h>

#include "sgx_error.h"       /* sgx_status_t */
#include "sgx_eid.h"     /* sgx_enclave_id_t */
#include "pbc/pbc.h"
#include "openssl/sha.h"
# define TOKEN_FILENAME   "enclave.token"
# define ENCLAVE_FILENAME "enclave.signed.so"

#define AES_TAG_SIZE          16
#define AES_IV_SIZE        12

extern sgx_enclave_id_t global_eid;    /* global enclave id */


#endif /* !_APP_H_ */
