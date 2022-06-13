#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */

#include "../Enclave/Enclave.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef O_PRINT_STR_DEFINED__
#define O_PRINT_STR_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, o_print_str, (const char* str));
#endif

sgx_status_t e_pairing_init(sgx_enclave_id_t eid, sgx_status_t* retval, char* s, size_t count);
sgx_status_t verify_signature(sgx_enclave_id_t eid, sgx_status_t* retval, char* msg, char* pub, char* sign);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
