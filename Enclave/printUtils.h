#ifndef PRINTF_UTIL_H
#define PRINTF_UTIL_H
#include "pbc/pbc.h"

void eprintf(const char *fmt, ...);
void printf_element_str(element_t e,unsigned char* name);
void printf_element(element_t e);
int number(int num);
#endif
