#include "kdebug.h"
#include "stdio.h"

void
print_kerninfo(void) {
	extern char etext[], edata[], end[], kern_init[];
	cprintf("------------------------\n");
	cprintf("Kernel Entry Point: 0x%08x \n", kern_init);
	cprintf("etext: 0x%08x \n", etext);
	cprintf("edata: 0x%08x \n", edata);
	cprintf("end: 0x%08x \n", end);
    cprintf("Kernel executable memory footprint: %dKB\n", (end - kern_init + 1023)/1024);
	cprintf("------------------------\n");
}
