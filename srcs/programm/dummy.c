#include <debugger.h>

int	main(void) {
	printf("Hello from programm\n");
	for (int i = 0; i < 100000; i++) {
		printf("%d\n", i);
	}
	printf("after loop\n");
	return (0);
}
