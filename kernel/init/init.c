#include "../../libs/defs.h"
#include "../../libs/x86.h"

void kern_init(void) __attribute__((noreturn));

void
kern_init(void){
    uint32_t a = 10;
    uint32_t b = 2;
    int i;
    for (i = 0; i < a; ++i) {
        b += i;
    }
    while(1);
}