/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Enable enclave debugging (SGX SDK) */
#define DEBUG 1

/* Enclave debugging in prerelease builds (SGX SDK) */
/* #undef EDEBUG */

/* Define to 1 if you have the `floor' function. */
#define HAVE_FLOOR 1

/* Define to 1 if you have the `gettimeofday' function. */
/* #undef HAVE_GETTIMEOFDAY */

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `pow' function. */
#define HAVE_POW 1

/* Define to 1 if you have the `sqrt' function. */
#define HAVE_SQRT 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strdup' function. */
/* #undef HAVE_STRDUP */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* No enclave debugging in release builds (SGX SDK) */
/* #undef NDEBUG */

/* Enable enclave debugging (Open Enclave) */
/* #undef OE_DEBUG_FLAG */

/* Name of package */
#define PACKAGE "libsgx_pbc"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "cyx.xjtu@gmail.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libsgx_pbc"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libsgx_pbc 0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libsgx_pbc"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1"

/* Enable hardware simulation mode */
/* #undef SGX_HW_SIM */

/* Define if building for SGX with OpenEnclave */
/* #undef SGX_WITH_OPENENCLAVE */

/* Define if building for SGX with the Intel SGX SDK */
#define SGX_WITH_SGXSDK 1

/* Version number of package */
#define VERSION "0.1"
