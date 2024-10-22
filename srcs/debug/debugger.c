#include <debugger.h>

typedef struct s_debugger {
	size_t	page_size;
}	t_debugger;

typedef	struct s_instruction {
	unsigned char	buffer[15];
	size_t			len;
}	t_instruction;

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

void	int3_sig_handler(int sig, siginfo_t *info, void *context_void) {
	printf("Breakpoint reached:\n");
	ucontext_t	*context = (ucontext_t *)context_void;
	long long int	*pc = context->uc_mcontext.gregs + REG_RIP;
	
	char	*line;

	line = readline("debugger: ");
	while (line) {
		free(line);
		line = readline("debugger: ");
	}
	(void)sig;
	(void)info;
	*replaced_program_location = replaced_program_byte;
	*pc -= 1;
}

void	setup_signals(t_debugger *debugger) {
	struct sigaction	sigact = {0};

	sigact.sa_sigaction = &int3_sig_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_SIGINFO;
	assert(sigaction(SIGTRAP, &sigact, NULL) != -1);
	(void)debugger;
}

t_debugger	init(void) {
	t_debugger	debugger;

	test_op_len();
	debugger.page_size = sysconf(_SC_PAGESIZE);
	setup_signals(&debugger);
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




int main() {
	t_debugger	debugger = init();
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

