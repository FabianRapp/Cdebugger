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

void	setup_first_breakpoint(t_debugger *debugger) {
	int status;

	ptrace(PTRACE_GETREGS, debugger->pid, NULL, &debugger->regs);
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
	check_child_status(debugger->pid);
	printf("inserting breakpoint \n");
	ptrace(PTRACE_GETREGS, debugger->pid, NULL, &debugger->regs);
	insert_breakpoint_here((void *)debugger->regs.rip, debugger);
	ERRNO_CHECK;

	printf("PRACE_CONT 1\n");
	ptrace(PTRACE_CONT, debugger->pid, NULL, NULL);
	ERRNO_CHECK;

	//WAIT for first break point
	waitpid(debugger->pid, &status, 0);
	printf("after waitpid for child after PTRACE_CONT (after first breakpoint)\n");
	ERRNO_CHECK;
	print_exit_status(status);

	if (1) {
		remove_cur_breakpoint(debugger);
		//breakpoint_handler(debugger);
	} else {
		ptrace(PTRACE_CONT, debugger->pid, 0, 0);
		waitpid(debugger->pid, &status, 0);
		printf("after waitpid\n");

		ptrace(PTRACE_CONT, debugger->pid, 0, 0);
		waitpid(debugger->pid, &status, 0);
		ERRNO_CHECK;
		printf("after waitpid\n");
		ERRNO_CHECK;
		ptrace(PTRACE_CONT, debugger->pid, 0, 0);
	}
	ERRNO_CHECK;
	int i = 0;
	while (1) {
		i++;
		waitpid(debugger->pid, &status, 0);
		assert(ptrace(PTRACE_CONT, debugger->pid, 0, 0) >= 0);
		//printf("%d\n", i);
	}
	ERRNO_CHECK;
}

void	fork_process(t_debugger *debugger, char **av, char **env) {
	ERRNO_CHECK;
	debugger->pid = fork();
	ERRNO_CHECK;
	assert("fork failed" && debugger->pid >= 0);
	if (debugger->pid == 0) {
		ERRNO_CHECK;
		printf("ptrace(PTRACE_TRACEME)\n");
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		printf("raise(SIGSTOP)\n");
		printf("execve %s\n", av[1]);
		execve(av[1], av + 1, env);
		assert(0 && "execve failed");
	} else {

		int status;
		ERRNO_CHECK;
		ptrace(PTRACE_ATTACH, debugger->pid, 0, 0);
		ERRNO_CHECK;
		waitpid(debugger->pid, &status, 0);
		if (WIFEXITED(status)) {
			printf("Child exited prematurely with status %d\n", WEXITSTATUS(status));
			return;
		}
		printf("after first waitpid\n");
		ERRNO_CHECK;
		print_exit_status(status);
		printf("Debugging %s with pid %d\n", av[1], debugger->pid);
		ERRNO_CHECK;
		ptrace(PTRACE_CONT, debugger->pid, 0, 0);
		waitpid(debugger->pid, &status, 0);
		if (WIFEXITED(status)) {
			printf("Child exited prematurely with status %d\n", WEXITSTATUS(status));
			return;
		}
		printf("after second waitpid\n");
		ERRNO_CHECK;
		print_exit_status(status);

		if (1) {
			setup_first_breakpoint(debugger);
		} else {
			ptrace(PTRACE_CONT, debugger->pid, 0, 0);
			waitpid(debugger->pid, &status, 0);
			if (WIFEXITED(status)) {
				printf("Child exited prematurely with status %d\n", WEXITSTATUS(status));
			}
			printf("after third waitpid\n");
			ERRNO_CHECK;
			print_exit_status(status);
		}
		ERRNO_CHECK;
	}
}
