#include <debugger.h>

void	check_child_status(pid_t child_pid) {
	int status;
	while (1) {
		pid_t result = waitpid(child_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
		if (result == 0) {
			// No change in child's status, non-blocking return
			printf("No change in child status.\n");
			break;
		} else if (result == -1) {
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		} else {
			// Check why waitpid() returned
			if (WIFEXITED(status)) {
				printf("Child exited with status %d.\n", WEXITSTATUS(status));
				break;
			}
			if (WIFSIGNALED(status)) {
				printf("Child killed by signal %d.\n", WTERMSIG(status));
				break;
			}
			if (WIFSTOPPED(status)) {
				printf("Child stopped by signal %d.\n", WSTOPSIG(status));
			}
			if (WIFCONTINUED(status)) {
				printf("Child continued.\n");
			}
		}
	}
}
