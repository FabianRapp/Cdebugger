#include <debugger.h>

void	fork_process(t_debugger *debugger, char **av, char **env) {
	debugger->pid = fork();
	assert("fork failed" && debugger->pid >= 0);
	if (debugger->pid == 0) {
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		raise(SIGSTOP);
		execve(av[1], av + 1, env);
	} else {
		int status;
		waitpid(debugger->pid, &status, 0);
		if (WIFSTOPPED(status)) {
			printf("Debugging %s with pid %d\n", av[1], debugger->pid);
			ptrace(PTRACE_CONT, debugger->pid, NULL, NULL);
		} else {
			assert("undexpeced child exit status" && 0);
		}
	}

}


