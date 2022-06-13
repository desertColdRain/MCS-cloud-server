#include <stdio.h>
#include "printUtils.h"
#include "../build/Enclave_t.h"

void eprintf(const char *fmt, ...){
  char buf[BUFSIZ] = {'\0'};
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  o_print_str(buf);
}

void printf_element_str(element_t e,unsigned char* name){
  uint32_t e_len=element_length_in_bytes(e);
  unsigned char e_str[e_len];
  
  element_to_bytes(e_str,e);
  eprintf("%s\n",name);
  for (size_t i = 0; i < e_len; i++) {
    eprintf("%02x ",e_str[i]);
  }
    eprintf("\n");
}


void printf_element(element_t e){
  //uint32_t e_len=element_length_in_bytes(e);
  eprintf("%s\n",e->field->data);
  
  eprintf("\n");
}

int number(int num)
{
    int i=0;
    do
    {
        num=num/10;
        i++;
    }while(num!=0);
    eprintf("这个数有 %d 位\n",i);
    return i;
}

// sgx_status_t writeSKE(mldp_ske* ske,size_t ith_level){
//   sgx_status_t ret=SGX_SUCCESS;
//   if (ith_level<1) {
//     ret=SGX_ERROR_UNEXPECTED;
//     return ret;
//   }
//   //initialize g_ske
//   g_ske[ith_level-1]=(mldp_ske*)malloc(sizeof(mldp_ske));
//   g_ske[ith_level-1]=ske;
// }
