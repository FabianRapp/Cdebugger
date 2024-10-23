#include <debugger.hpp>

Breakpoint	*bp = NULL;

void print_exit_status(int status) {
	if (WIFSIGNALED(status)) {
		PRINT_YELLOW("Exited due to uncaught signal: " << WEXITSTATUS(status));
	} else if (WIFEXITED(status)) {
		PRINT_YELLOW("Exited normally with exit code " << WIFEXITED(status));
	} else if (WIFSTOPPED(status)) {
		PRINT_YELLOW("Stopped by signal: " << WSTOPSIG(status));
	} else if (WIFCONTINUED(status)) {
		PRINT_YELLOW("Continued");
	} else {
		PRINT_YELLOW("Exited with unknown status");
	}
}

void	setup_first_breakpoint(Debugee &debugee) {
	ERRNO_CHECK;

	//printf("inserting breakpoint \n");

	//debugee.step();
	//debugee.wait();



	t_program_ptr pc = debugee.get_pc();
	bp = debugee.new_bp(pc);

	//ERRNO_CHECK;

	//printf("PRACE_cont 1\n");
	//debugee.cont();
	//ERRNO_CHECK;
	//debugee.wait();
	//pc = debugee.get_pc();
	//pc--;
	//debugee.set_pc(pc);
	//debugee.cont();
	//debugee.wait();
	//ERRNO_CHECK;
	while (!debugee.finished()) {
		breakpoint_handler(debugee);
		ERRNO_CHECK;
		debugee.wait();
		ERRNO_CHECK;
	}

	ERRNO_CHECK;
}

void	fork_process(t_debugger *debugger, char **av, char **env) {
	ERRNO_CHECK;

	Debugee debugee(av[1], av + 1, env);
	printf("after first waitpid\n");
	ERRNO_CHECK;
	printf("after second waitpid\n");
	ERRNO_CHECK;

	setup_first_breakpoint(debugee);
}
