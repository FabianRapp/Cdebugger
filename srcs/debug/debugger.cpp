#include <debugger.hpp>
#include <Debugee.hpp>

sigset_t	tmp_blocked_sigs;
t_debugger	debugger;


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

void	signal_handler_ctrl_c(int sig, siginfo_t *info, void *vcontext) {
	ucontext_t	*context = (ucontext_t *)vcontext;
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
	//sig_handler_flag = 1;
	(void)context;
	(void)sig;
	(void)info;
}

void	set_ctrl_c(void) {
	struct sigaction	sig;

	sigaddset(&tmp_blocked_sigs, SIGINT);
	bzero(&sig, sizeof sig);
	sigemptyset(&(sig.sa_mask));
	sig.sa_flags = SA_SIGINFO;
	sig.sa_sigaction = signal_handler_ctrl_c;
	if (sigaction(SIGINT, &sig, NULL) == -1)
		assert(0);
}

void	set_signals(void) {
	sigemptyset(&tmp_blocked_sigs);
	set_ctrl_c();
	bzero(&tmp_blocked_sigs, sizeof tmp_blocked_sigs);
}

// only for defined singlas, rather than all
void	block_signals(void) {
	sigprocmask(SIG_BLOCK, &tmp_blocked_sigs, NULL);
}

// only for defined singlas, rather than all
void	unblock_signals(void) {
	sigprocmask(SIG_UNBLOCK, &tmp_blocked_sigs, NULL);
}

t_debugger	init(int ac, char **av, char **env) {
	test_op_len();
	bzero(&debugger, sizeof debugger);
	assert("need atleast 1 argument: <executable> or <pid + anything>" && ac > 1);
	debugger.page_size = sysconf(_SC_PAGESIZE);
	set_signals();
	if (ac == 2)
		debugger.debugee = new Debugee(av[1], av + 1, env);
	else if (ac == 3) {
		debugger.debugee = new Debugee((pid_t)atoi(av[1]));
	} else {
		assert("invlaid ac" && 0);
	}
	//breakpoint_init_print();
	return (debugger);
}

void	delete_debugee(void) {
	delete debugger.debugee;
}

int main(int ac, char *av[], char *env[]) {
	debugger = init(ac, av, env);
	atexit(delete_debugee);
	//debugger.debugee->cont();
	printf("entering main loop\n");
	while (!debugger.debugee->finished()) {
		breakpoint_handler(*debugger.debugee);
		debugger.debugee->wait();
		if (debugger.debugee->finished()) {
			break ;
		}
		//uint8_t	buf[8];
		////todo: make fn in Debugee that verifys addresses for operations based of maps
		//debugger.debugee->read_data(debugger.debugee->get_reg(RSP), buf, sizeof buf);
		////debugger.debugee->read_data(0x7ffffffdd000, buf, sizeof buf);
		//for (size_t i = 0; i < sizeof buf; i++) {
		//	std::cout << std::hex << (size_t)buf[i] << "|";
		//}
		//std::cout << std::endl;
		//debugger.debugee->set_reg(RIP, debugger.debugee->_memmaps.ranges[0].start);
	}
	//int status;
	//waitpid(debugger.pid,&status, 0);
	//delete debugger.debugee;
	return (0);
}

