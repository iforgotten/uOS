#include <stdio.h>

int main() {
	printf("# vectors\n");
	printf(".text\n");
	printf(".global __alltraps\n");

	int i;
	for(i = 0; i < 256; ++i) {
		printf(".global vector%d\n",i);
		printf("vector%d:\n",i);
		if((i < 8 || i > 14) && i != 17) {
			printf("  pushl $0\n");
		}
		printf("  pushl $%d\n",i);
		printf("  jmp __alltraps\n");
    }
    printf("\n");
    printf("# vector table");
    printf(".data\n");
    printf(".global __vectors\n");
    printf("__vectors:\n");

    int j;
    for(j = 0; j < 255; ++j) {
        printf("  .long vector%d\n",j);
    }
    printf("  .long vector255");
    return 0;
}
