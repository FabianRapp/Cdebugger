#include <debugger.h>

// return false incase the programm should continue
bool	handle_input(t_debugger *debugger, char *line) {
	if (!strncmp(line, "continue", strlen("continue"))) {
		ptrace(PTRACE_POKETEXT, debugger->pid);
		ptrace(PTRACE_CONT, debugger->pid);
		return (false);
	} else if (!strncmp(line, "REGS", strlen("REGS"))) {
		printf("not active\n");
		//printf("PC: %lld, RAX: %lld, RBX: %lld, RCX: %lld, RDX: %lld\n",
		//	context->uc_mcontext.gregs[REG_RIP] - 1,
		//	context->uc_mcontext.gregs[REG_RAX],
		//	context->uc_mcontext.gregs[REG_RBX],
		//	context->uc_mcontext.gregs[REG_RCX],
		//	context->uc_mcontext.gregs[REG_RDX]);
	} else if (!strncmp(line, "n", 1)
		|| !strncmp(line, "next", strlen("next"))) {
		printf("not active\n");
		//*replaced_program_location = replaced_program_byte;
		//context->uc_mcontext.gregs[REG_RIP] -= 1;
		//size_t	instruct_len = op_len(replaced_program_location);
		//context->uc_mcontext.gregs[REG_RIP] += instruct_len;
		//instruct_len = op_len((void *)(context->uc_mcontext.gregs[REG_RIP]));
		//insert_breakpoint_here((void *)(context->uc_mcontext.gregs[REG_RIP]) + instruct_len);
		return (false);
	}
	return (true);
}

void	breakpoint_handler(t_debugger *debugger) {
	printf("Breakpoint reached:\n");
	char	*line = readline("debugger: ");
	while (line) {
		if (!handle_input(debugger, line))
			break ;
		free(line);
		line = readline("debugger: ");
	}
	free(line);
}
