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
