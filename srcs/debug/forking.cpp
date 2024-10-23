#include <debugger.hpp>

void print_exit_status(int status) {
	if (WIFSIGNALED(status)) {
		printf("Exited due to uncaught signal: %d\n", WTERMSIG(status));
	} else if (WIFEXITED(status)) {
		printf("Exited normally with exit code %d\n", WEXITSTATUS(status));
	} else if (WIFSTOPPED(status)) {
		printf("Stopped by signal: %d\n", WSTOPSIG(status));
	} else if (WIFCONTINUED(status)) {
		printf("Continued\n");
	} else {
		printf("Exited with unknown status\n");
	}
}

void	setup_first_breakpoint(Debugee &debugee) {
	ERRNO_CHECK;

	//for (int i = 3000; i; i--)
	//{

	//	ERRNO_CHECK;
	//	ptrace(PTRACE_SINGLESTEP, debugger->pid, 0, 0);
	//	ERRNO_CHECK;

	//	//WAIT for step
	//	waitpid(debugger->pid, &status, 0);
	//	if (WIFEXITED(status)) {
	//		printf("Child exited prematurely with status %d\n", WEXITSTATUS(status));
	//		return;
	//	}

	//	ptrace(PTRACE_GETREGS, debugger->pid, NULL, &debugger->regs);
	//	ERRNO_CHECK;
	//	//printf("after waitpid single step\n");
	//	//printf("PC: %lld\n", debugger->regs.rip);
	//	ERRNO_CHECK;
	//	//print_exit_status(status);
	//}
	
	printf("inserting breakpoint \n");
	Breakpoint	*bp = debugee.new_bp(debugee.get_pc());
	ERRNO_CHECK;

	printf("PRACE_CONT 1\n");
	debugee.cont();
	ERRNO_CHECK;

	//WAIT for first break point
	debugee.wait();
	printf("after waitpid for child after PTRACE_CONT (after first breakpoint)\n");
	ERRNO_CHECK;

	if (1) {
		std::cout << "delete bp" << std::endl;
		delete	bp;
		std::cout << "cont" << std::endl;
		debugee.cont();
		//breakpoint_handler(debugger);
	} else {
		debugee.cont();
		debugee.wait();
		printf("after waitpid\n");
		ERRNO_CHECK;
	
		debugee.cont();
		debugee.wait();
		printf("after waitpid\n");
		ERRNO_CHECK;

		debugee.cont();
		ERRNO_CHECK;
	}
	ERRNO_CHECK;
}

void	fork_process(t_debugger *debugger, char **av, char **env) {
	ERRNO_CHECK;
	debugger->pid = fork();
	assert("fork failed" && debugger->pid >= 0);
	if (debugger->pid == 0) {
		ERRNO_CHECK;
		PRINT_YELLOW("ptrace(PTRACE_TRACEME)");
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		PRINT_YELLOW("raise(SIGSTOP)");
		PRINT_YELLOW("execve %s" << av[1]);
		execve(av[1], av + 1, env);
		assert(0 && "execve failed");
	} else {
		Debugee debugee(debugger->pid);
		printf("Debugging %s with pid %d\n", av[1], debugger->pid);
		debugee.wait();
		printf("after first waitpid\n");
		ERRNO_CHECK;
		debugee.cont();
		debugee.wait();
		printf("after second waitpid\n");
		ERRNO_CHECK;

		setup_first_breakpoint(debugee);
		ERRNO_CHECK;
		debugee.wait();
	}
}
