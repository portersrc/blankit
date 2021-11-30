#define  _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>                                                              
#include <fcntl.h>                                                              
#include <sys/stat.h>
#include "blankit.h"

#ifdef BLANKIT_DBG
#define blankit_printf_dbg(...) printf(__VA_ARGS__)
#else
#define blankit_printf_dbg(...)
#endif

void blankit_predict(int predict_idx)
{
    printf("blankit_predict stub\n"); // XXX don't remove this. The probe
                                      // requires (and replaces) this function.
    //blankit_printf_dbg("blankit_predict stub. Probe should replace this.\n");
    //blankit_printf_dbg("  predict_set: %s\n",predict_set);
}
