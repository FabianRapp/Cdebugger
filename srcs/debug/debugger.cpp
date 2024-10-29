#include <debugger.hpp>
#include <Debugee.hpp>

uint8_t		*replaced_program_location;
uint64_t	replaced_word;

t_debugger	debugger;
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
	std::string	options[] = {"continue", "REGS", "n/next"};
	printf("Options:\n");
	for (size_t i = 0; i < sizeof options / sizeof options[0]; i++) {
		std::cout << "\t" << options[i] << std::endl;
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

void	signal_handler_ctrl_c(int sig) {
	(void)sig;
	char	*input = NULL;
	printf("'EXIT' or ctrl+d to exit\n");
	input = readline("INPUT: ");
	if (!input || !strncmp(input, "EXIT", strlen("EXIT"))) {
		exit(0);
	}
	if (!strcmp(input, "p") || !strcmp(input, "pause")) {
		printf("pause!\n");
		kill(debugger.debugee->get_pid(), SIGTRAP);
	}
	free(input);
}

void	set_ctrl_c(void)
{
	struct sigaction	sig;

	sigemptyset(&(sig.sa_mask));
	sig.sa_flags = 0;
	sig.sa_handler = signal_handler_ctrl_c;
	if (sigaction(SIGINT, &sig, NULL) == -1)
		assert(0);
}
t_debugger	init(int ac, char **av, char **env) {
	test_op_len();
	assert("need 1 argument: executable" && ac > 1);
	debugger.page_size = sysconf(_SC_PAGESIZE);
	set_ctrl_c();
	if (ac == 2)
		fork_process(&debugger, av, env);
	else if (ac == 3) {
		debugger.debugee = new Debugee((pid_t)atoi(av[1]));
	} else {
		assert("invlaid ac" && 0);
	}
	//breakpoint_init_print();
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
	insert_breakpoint_here((uint8_t *)fn, debugger);
	result = fn();
	printf("Result of the machine code: %d\n", result);
	free((void *)fn);
}


int main(int ac, char *av[], char *env[]) {
	debugger = init(ac, av, env);
	//debugger.debugee->cont();
	printf("entering main loop\n");
	while (!debugger.debugee->finished()) {
		breakpoint_handler(*debugger.debugee);
		debugger.debugee->wait();
		uint8_t	buf[100];
		//todo: make fn in Debugee that verifys addresses for operations based of maps
		debugger.debugee->read_data(debugger.debugee->get_reg(RSP), buf, sizeof buf);
		//debugger.debugee->read_data(0x7ffffffdd000, buf, sizeof buf);
		for (size_t i = 0; i < sizeof buf; i++) {
			std::cout << std::hex << (size_t)buf[i] << "|";
		}
		std::cout << std::endl;
		//debugger.debugee->set_reg(RIP, debugger.debugee->_memmaps.ranges[0].start);
	}
	//int status;
	//waitpid(debugger.pid,&status, 0);
	delete debugger.debugee;
	return 0;
}

