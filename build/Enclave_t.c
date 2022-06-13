#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


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

static sgx_status_t SGX_CDECL sgx_e_pairing_init(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_pairing_init_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_pairing_init_t* ms = SGX_CAST(ms_e_pairing_init_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_s = ms->ms_s;
	size_t _tmp_count = ms->ms_count;
	size_t _len_s = _tmp_count;
	char* _in_s = NULL;

	CHECK_UNIQUE_POINTER(_tmp_s, _len_s);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_s != NULL && _len_s != 0) {
		if ( _len_s % sizeof(*_tmp_s) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_s = (char*)malloc(_len_s);
		if (_in_s == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_s, _len_s, _tmp_s, _len_s)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}

	ms->ms_retval = e_pairing_init(_in_s, _tmp_count);

err:
	if (_in_s) free(_in_s);
	return status;
}

static sgx_status_t SGX_CDECL sgx_verify_signature(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_verify_signature_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_verify_signature_t* ms = SGX_CAST(ms_verify_signature_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_msg = ms->ms_msg;
	char* _tmp_pub = ms->ms_pub;
	char* _tmp_sign = ms->ms_sign;



	ms->ms_retval = verify_signature(_tmp_msg, _tmp_pub, _tmp_sign);


	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[2];
} g_ecall_table = {
	2,
	{
		{(void*)(uintptr_t)sgx_e_pairing_init, 0, 0},
		{(void*)(uintptr_t)sgx_verify_signature, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][2];
} g_dyn_entry_table = {
	1,
	{
		{0, 0, },
	}
};


sgx_status_t SGX_CDECL o_print_str(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_o_print_str_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_o_print_str_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(str, _len_str);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (str != NULL) ? _len_str : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_o_print_str_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_o_print_str_t));
	ocalloc_size -= sizeof(ms_o_print_str_t);

	if (str != NULL) {
		ms->ms_str = (const char*)__tmp;
		if (_len_str % sizeof(*str) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, str, _len_str)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_str);
		ocalloc_size -= _len_str;
	} else {
		ms->ms_str = NULL;
	}
	
	status = sgx_ocall(0, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

