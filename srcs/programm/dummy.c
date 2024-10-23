#include <debugger.hpp>

void	int3_sig_handler(int sig, siginfo_t *as, void *conext_void) {
	ucontext_t	*context = (ucontext_t *) conext_void;
	printf("CHILD PC: %lld\n", context->uc_mcontext.gregs[REG_RIP]);
	(void)as;
}

void	setup_sigtrap(void) {
	struct sigaction	sigact = {0};

	sigact.sa_sigaction = &int3_sig_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_SIGINFO;
	assert(sigaction(SIGTRAP, &sigact, NULL) != -1);
}
int	main(void) {
	//setup_sigtrap();
	BASIC_BREAK;
	printf("Hello from programm\n");
	for (int i = 0; i < 3; i++) {
		printf("%d\n", i);
	}
	printf("after loop\n");
	return (0);
}
