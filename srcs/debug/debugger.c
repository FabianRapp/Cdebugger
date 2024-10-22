#include <debugger.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <capstone/capstone.h>

typedef	struct s_instruction {
	unsigned char	buffer[15];
	size_t			len;
}	t_instruction;

int main() {
	// Machine code for: mov eax, 1; ret
	//						|int3	|mov eax, 1						|ret	|int3	|noop
	unsigned char code[] = {0xcc,	0xB8, 0x01, 0x00, 0x00, 0x00,	0xC3,	0xcc,	NOP_OPCODE};
	void *buf;

	test_op_len();

	// Allocate a memory page that is both executable and writable
	size_t pagesize = sysconf(_SC_PAGESIZE);
	posix_memalign(&buf, pagesize, pagesize);
	mprotect(buf, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);

	memcpy(buf, code, sizeof(code));

	test_op_len();
	size_t	len[30] = {0};
	uint8_t	*ptr = (uint8_t *)buf;
	len[0] = op_len(ptr);
	ptr += len[0];
	len[1] = op_len(ptr);
	ptr += len[1];
	len[2] = op_len(ptr);
	ptr += len[2];
	len[3] = op_len(ptr);
	ptr += len[3];
	len[4] = op_len(ptr);
	printf("len[0]: %lu\n", len[0]);
	printf("len[1]: %lu\n", len[1]);
	printf("len[2]: %lu\n", len[2]);
	printf("len[3]: %lu\n", len[4]);

	// Execute the machine code
	//int (*func)() = buf;
	//int result = func();

	//printf("Result of the machine code: %d\n", result);

	free(buf);
	return 0;
}

