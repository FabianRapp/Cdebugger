#include <debugger.hpp>

uint8_t		*replaced_program_location;
uint64_t	replaced_word;

// return the removed instruction byte
void	insert_breakpoint_here(uint8_t *program, t_debugger *debugger) {
	{
		replaced_program_location = program;
		printf("inserting at %p\n", program);
		printf("inserting at %p\n", program);
		replaced_word = ptrace(PTRACE_PEEKTEXT, debugger->pid, debugger->regs.rip, NULL);
		printf("old word: %16lx\n", replaced_word);
	}
	{
		const size_t shift_size = ((sizeof (uint64_t)) - 1) * 8;
		uint64_t	opcode = ((uint8_t)INT3_OPCODE);
		opcode <<= shift_size;
		uint64_t	new_word = 0xFF; 
		new_word <<= shift_size;
		new_word = ~new_word;
		new_word &= replaced_word;
		new_word |= opcode;
		printf("new word: %16lx\n", new_word);

		ptrace(PTRACE_POKETEXT, debugger->pid, debugger->regs.rip, new_word);
	}
}

void	breakpoint_init_print(void) {
	char	*options[] = {"continue", "REGS", "n/next"};
	printf("Options:\n");
	for (size_t i = 0; i < sizeof options / sizeof options[0]; i++) {
		printf("\t%s\n", options[i]);
	}
	printf("Cntr+d is undefined behaivior\n");
}

//void	setup_sigtrap(void) {
//	struct sigaction	sigact = {0};
//
//	sigact.sa_sigaction = &int3_sig_handler;
//	sigemptyset(&sigact.sa_mask);
//	sigact.sa_flags = SA_SIGINFO;
//	assert(sigaction(SIGTRAP, &sigact, NULL) != -1);
//}

t_debugger	init(int ac, char **av, char **env) {

	t_debugger	debugger;

	test_op_len();
	assert("need 1 argument: executable" && ac > 1);
	debugger.page_size = sysconf(_SC_PAGESIZE);
	//breakpoint_init_print();
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

void	old_test(t_debugger *debugger) {
	int	(*fn)(void);
	int	result;

	fn = alloc_dummy_fn(*debugger);
	result = fn();
	printf("Result of the machine code: %d\n", result);
	insert_breakpoint_here((void *)fn, debugger);
	result = fn();
	printf("Result of the machine code: %d\n", result);
	free(fn);
}


int main(int ac, char *av[], char *env[]) {
	t_debugger	debugger = init(ac, av, env);
	//int status;
	//waitpid(debugger.pid,&status, 0);
	(void)debugger;
	return 0;
}

