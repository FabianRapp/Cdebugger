#include <debugger.h>

void	setup_first_breakpoint(t_debugger *debugger) {
	ptrace(PTRACE_GETREGS, debugger->pid, NULL, &debugger->regs);
	uint8_t	*cur_pc = (uint8_t *) debugger->regs.rip;
	ERRNO_CHECK;
	printf("inserting breakpoint \n");
	insert_breakpoint_here(cur_pc, debugger);
	printf("PRACE_CONT 1\n");
	ptrace(PTRACE_CONT, debugger->pid, NULL, NULL);

	int status;
	printf("waiting for child after PTRACE_CONT\n");
	//WAIT for first break point
	waitpid(debugger->pid, &status, 0);
	breakpoint_handler(debugger);
    if (WIFEXITED(status)) {
        printf("Process exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("Process stopped unexpectedly\n");
    }
}

void	fork_process(t_debugger *debugger, char **av, char **env) {
	debugger->pid = fork();
	assert("fork failed" && debugger->pid >= 0);
	if (debugger->pid == 0) {
		printf("ptrace(PTRACE_TRACEME)\n");
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		//printf("raise(SIGSTOP)\n");
		raise(SIGSTOP);
		printf("execve %s\n", av[1]);
		execve(av[1], av + 1, env);
	} else {
		int status;
		//wait for PTRACE_TRACEME
		//waitpid(debugger->pid, &status, 0);
		//wait for raise(SIGSTOP)
		waitpid(debugger->pid, &status, 0);
		ptrace(PTRACE_CONT, debugger->pid, NULL, NULL);
		if (WIFSTOPPED(status)) {
			printf("Debugging %s with pid %d\n", av[1], debugger->pid);
			setup_first_breakpoint(debugger);
		} else {
			assert("undexpeced child exit status" && 0);
		}
	}
}
