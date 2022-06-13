#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_e_pairing_init_t {
	sgx_status_t ms_retval;
	char* ms_s;
	size_t ms_count;
} ms_e_pairing_init_t;

typedef struct ms_verify_signature_t {
	sgx_status_t ms_retval;
	char* ms_msg;
	char* ms_pub;
	char* ms_sign;
} ms_verify_signature_t;

typedef struct ms_o_print_str_t {
	const char* ms_str;
} ms_o_print_str_t;

static sgx_status_t SGX_CDECL Enclave_o_print_str(void* pms)
{
	ms_o_print_str_t* ms = SGX_CAST(ms_o_print_str_t*, pms);
	o_print_str(ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Enclave = {
	1,
	{
		(void*)Enclave_o_print_str,
	}
};
sgx_status_t e_pairing_init(sgx_enclave_id_t eid, sgx_status_t* retval, char* s, size_t count)
{
	sgx_status_t status;
	ms_e_pairing_init_t ms;
	ms.ms_s = s;
	ms.ms_count = count;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t verify_signature(sgx_enclave_id_t eid, sgx_status_t* retval, char* msg, char* pub, char* sign)
{
	sgx_status_t status;
	ms_verify_signature_t ms;
	ms.ms_msg = msg;
	ms.ms_pub = pub;
	ms.ms_sign = sign;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

