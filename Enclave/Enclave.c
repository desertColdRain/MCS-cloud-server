#include <stdio.h>
#include "string.h"
#include "sgx_trts.h"
#include "../build/Enclave_t.h"
#include "pbc/pbc.h"
#include "printUtils.h"
//#include "openssl/sha.h"


sgx_status_t e_pairing_init(char* param, size_t count){
        sgx_status_t ret = SGX_SUCCESS;
     if(count<0){
             eprintf("[Err]: parameter count error\n");
                ret = SGX_ERROR_UNEXPECTED;
                return ret;
        }
        if(pairing_init_set_buf(pairing, param, count)) {
                eprintf("[Err]: pairing init failed\n");
                ret = SGX_ERROR_UNEXPECTED;
        }
        element_init_G1(g_g,pairing);
        element_random(g_g);
        printf_element_str(g_g,"g_g");
        memset(ID_nonfirst,0,MAX_GET_TOKENI_NUM+1);

        element_init_G2(g, pairing);
        element_init_G2(public_key, pairing);
        element_init_G1(h, pairing);
        element_init_G1(sig, pairing);
        element_init_GT(temp1, pairing);
        element_init_GT(temp2, pairing);
        element_init_Zr(secret_key, pairing);

        //the random element of algrithm in client
        const char *p = "[2514483320253500302830735518607656714308729631627140256127950789857791084170321210748432578455411432835351305021305213948374733591027660304698451914113760,1465576007247186375635850908754396572575892168435373480940139024985694316088842090794484785227575005232364936905674489345374772001536876782497782554345558,0]";
        //const char *pks = "[7863279498809532165600087376174674527230246099016539067634937675912028763196198050841760764745629559919180594692274460066696356010157205688025851382815421,4979349996301213707382530085793108997147706483997137704517537359089793872887317868247593171944864716176948829150102448774519341430723400375943438058727160,0]";
        //element_set_str(public_key,pks,10);
        //element_random(g);
        element_set_str(g,p,10);
        printf_element_str(g,"g");
        //generate secret key
        //element_random(secret_key);
        char *s = "410938863709416111975378970886384826198899178968";
        //
        element_set_str(secret_key,s,10);
        printf_element_str(secret_key,"Secret key");
        //generate corresponding public key
        element_pow_zn(public_key, g, secret_key);
        printf_element_str(public_key,"Public key:");
        //eprinft(public_key);
        element_from_hash(h, "hello,world", strlen("hello,world"));
        
        printf_element_str(h,"hash***");
        element_pow_zn(sig, h, secret_key);
        //pairing_apply(temp1, sig, g, pairing);
        //pairing_apply(temp2, h, public_key, pairing);
       /* if (!element_cmp(temp1, temp2)) {
                eprintf("signature verifies\n");
        } else {
                eprintf("signature does not verify\n");
        }
*/
        //printf_element_str(secret_key,"Secret key:");
        if (ret!=SGX_SUCCESS) {
                eprintf("PairingInit failed");
        }
        return ret;
}


sgx_status_t verify_signature(char* msg, char *pub,char *sign)
{
        sgx_status_t ret = SGX_SUCCESS;
        element_init_G1(hash,pairing);
        element_init_G2(public, pairing);
        element_init_G1(signature, pairing);
        //init the public key from the parameters accepted by socket server
        element_set_str(public,pub,10);
        //the same as above, init the signature
        element_set_str(signature,sign,10);
        //caculate the hash of message that is designed for signature
        element_from_hash(hash, msg, strlen(msg));
        printf_element_str(hash,"hash in verify signature");

        printf_element_str(public,"public key after decode");
       printf_element_str(signature,"signature after decode");

       printf_element_str(g,"verify g");
        //to verify the signature
        pairing_apply(temp1, signature, g, pairing);
        pairing_apply(temp2, hash, public, pairing);

        if (!element_cmp(temp1, temp2)) {
                eprintf("signature verifies\n");
                return SGX_SUCCESS;
        
        } else {
                eprintf("signature does not verify\n");
                return SGX_ERROR_UNEXPECTED;
        }
        return ret;
}

