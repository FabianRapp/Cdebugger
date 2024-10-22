#include <debugger.h>

extern uint8_t	*replaced_program_location;
extern uint8_t	replaced_program_byte;

// return false incase the programm should continue
bool	handle_input(char *line, ucontext_t *context) {
	if (!strncmp(line, "continue", strlen("continue"))) {
		*replaced_program_location = replaced_program_byte;
		context->uc_mcontext.gregs[REG_RIP] -= 1;
		return (false);
	} else if (!strncmp(line, "REGS", strlen("REGS"))) {
		printf("PC: %lld, RAX: %lld, RBX: %lld, RCX: %lld, RDX: %lld\n",
			context->uc_mcontext.gregs[REG_RIP] - 1,
			context->uc_mcontext.gregs[REG_RAX],
			context->uc_mcontext.gregs[REG_RBX],
			context->uc_mcontext.gregs[REG_RCX],
			context->uc_mcontext.gregs[REG_RDX]);
	} else if (!strncmp(line, "n", 1)
		|| !strncmp(line, "next", strlen("next"))) {
		*replaced_program_location = replaced_program_byte;
		context->uc_mcontext.gregs[REG_RIP] -= 1;
		size_t	instruct_len = op_len(replaced_program_location);
		context->uc_mcontext.gregs[REG_RIP] += instruct_len;
		instruct_len = op_len((void *)(context->uc_mcontext.gregs[REG_RIP]));
		insert_breakpoint_here((void *)(context->uc_mcontext.gregs[REG_RIP]) + instruct_len);
		return (false);
	}
	return (true);
}

void	int3_sig_handler(int sig, siginfo_t *info, void *context) {
	char	*line = readline("debugger: ");
	while (line) {
		if (!handle_input(line, context))
			break ;
		free(line);
		line = readline("debugger: ");
	}
	free(line);

	(void)sig;
	(void)info;
}
