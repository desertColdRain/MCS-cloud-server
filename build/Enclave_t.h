#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "../Enclave/Enclave.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

sgx_status_t e_pairing_init(char* s, size_t count);
sgx_status_t verify_signature(char* msg, char* pub, char* sign);

sgx_status_t SGX_CDECL o_print_str(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
