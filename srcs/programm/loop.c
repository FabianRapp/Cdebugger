//#include <debugger.hpp>
/*
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
*/
#include <unistd.h>
#include <string.h>
int	my_strlen(char *str) {
	int i = 0;
	while (str[i]) {
		i++;
	}
	return (i);
}

int	main(void) {
	//setup_sigtrap();
	//BASIC_BREAK;

	char	hello[] = "Hello from programm\n";
	write (1, hello, my_strlen(hello));
	int		i = 0;
	char	str[16] = {0};
	strcpy(str, "hi\n");
	while (1)
	{
		write(1, str, strlen(str));
		usleep(100000);
		i++;
		i%= 10;
	}
	return (0);
}
