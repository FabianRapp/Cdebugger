#include <debugger.h>

uint8_t	*replaced_program_location;
uint8_t	replaced_program_byte;

// return the removed instruction byte
uint8_t	insert_breakpoint_here(uint8_t *program) {
	uint8_t		old;

	replaced_program_location = program;
	old = program[0];
	program[0] = INT3_OPCODE;
	return (old);
}

void	breakpoint_init_print(void) {
	char	*options[] = {"continue", "REGS", "n/next"};
	printf("Breakpoint reached:\n");
	printf("Options:\n");
	for (size_t i = 0; i < sizeof options / sizeof options[0]; i++) {
		printf("\t%s\n", options[i]);
	}
	printf("Cntr+d is undefined behaivior\n");
}

void	setup_sigtrap(void) {
	struct sigaction	sigact = {0};

	sigact.sa_sigaction = &int3_sig_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_SIGINFO;
	assert(sigaction(SIGTRAP, &sigact, NULL) != -1);
}

t_debugger	init(int ac, char **av, char **env) {

	t_debugger	debugger;

	test_op_len();
	assert("need 1 argument: executable" && ac > 1);
	debugger.page_size = sysconf(_SC_PAGESIZE);
	breakpoint_init_print();
	setup_sigtrap();
	fork_process(&debugger, av, env);
	return (debugger);
}

int	(*alloc_dummy_fn(t_debugger debugger))(void) {
	//						|mov eax, 1						|noop		|ret
	unsigned char code[] = {0xB8, 0x01, 0x00, 0x00, 0x00,	NOP_OPCODE, 0xC3};
	void *buf;
	posix_memalign(&buf, debugger.page_size, debugger.page_size);
	mprotect(buf, debugger.page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
	memcpy(buf, code, sizeof(code));
	return ((int (*)(void))buf);
}

int main(int ac, char *av[], char *env[]) {
	t_debugger	debugger = init(ac, av, env);

	int	(*fn)(void);
	int	result;

	fn = alloc_dummy_fn(debugger);
	result = fn();
	printf("Result of the machine code: %d\n", result);
	replaced_program_byte = insert_breakpoint_here((void *)fn);
	result = fn();
	printf("Result of the machine code: %d\n", result);

	free(fn);
	return 0;
}

