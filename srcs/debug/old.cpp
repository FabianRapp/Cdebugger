int	(*alloc_dummy_fn(t_debugger debugger))(void) {
	//						|mov eax, 1						|noop		|ret
	unsigned char code[] = {0xB8, 0x01, 0x00, 0x00, 0x00,	NOP_OPCODE, 0xC3};
	void *buf;
	posix_memalign(&buf, debugger.page_size, debugger.page_size);
	mprotect(buf, debugger.page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
	memcpy(buf, code, sizeof(code));
	return ((int (*)(void))buf);
}
void	remove_cur_breakpoint(t_debugger *debugger) {
	assert(ptrace(PTRACE_GETREGS, debugger->pid, 0, &debugger->regs) >= 0);
	printf("old replace loc: %p\n", replaced_program_location);
	printf("cur : %p\n", (void *)(debugger->regs.rip));
	//debugger->regs.rip = (long int)replaced_program_location + 1;
	debugger->regs.rip -= 1;
	ERRNO_CHECK;
	ptrace(PTRACE_SETREGS, debugger->pid, 0, &debugger->regs);
	ERRNO_CHECK;
	ptrace(PTRACE_GETREGS, debugger->pid, 0, &debugger->regs);

	ERRNO_CHECK;
	ptrace(PTRACE_POKETEXT, debugger->pid, debugger->regs.rip, 0xc88e8e78948);
	ERRNO_CHECK;
	ptrace(PTRACE_CONT, debugger->pid, 0, 0);
	ERRNO_CHECK;
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
// return the removed instruction byte
void	insert_breakpoint_here(uint8_t *program, t_debugger *debugger) {
	{
		replaced_program_location = program;
		printf("inserting at %p\n", program);
		printf("inserting at %p\n", program);
		replaced_word = ptrace(PTRACE_PEEKTEXT, debugger->pid, debugger->regs.rip, NULL);
		printf("old word: %16lx\n", replaced_word);
	}
	{
		const size_t shift_size = ((sizeof (uint64_t)) - 1) * 8;
		uint64_t	opcode = ((uint8_t)INT3_OPCODE);
		opcode <<= shift_size;
		uint64_t	new_word = 0xFF; 
		new_word <<= shift_size;
		new_word = ~new_word;
		new_word &= replaced_word;
		new_word |= opcode;
		printf("new word: %16lx\n", new_word);

		ptrace(PTRACE_POKETEXT, debugger->pid, debugger->regs.rip, new_word);
	}
}
