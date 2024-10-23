#include <debugger.hpp>

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

// return false incase the programm should continue
bool	handle_input(Debugee &debugee, char *line) {
	if (!strncmp(line, "continue", strlen("continue"))) {
		delete bp;
		debugee.cont();
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
		//PRINT_RED("bp: " << bp->get_pos());
		////delete bp;
		////todo: make this part of Debugee (Debugee::get_next_len())
		//unsigned char program_cpy[16 + sizeof(t_word)] = {0};
		//t_program_ptr	ptr = debugee.get_pc();
		//PRINT_RED("ptr: " << ptr);
		//t_word	*words = (t_word *)program_cpy;
		//for (size_t i = 0; i < (16 / sizeof(t_word)); i ++) {
		//	words[i] = debugee.get_word(ptr);
		//}
		//size_t	instruction_len = op_len((t_program_ptr)words);
		//t_program_ptr	pc = debugee.get_pc();
		////pc += instruction_len;
		////bp = debugee.new_bp(pc);

		//PRINT_RED("bp: " << bp->get_pos());

		//*replaced_program_location = replaced_program_byte;
		//context->uc_mcontext.gregs[REG_RIP] -= 1;
		//size_t	instruct_len = op_len(replaced_program_location);
		//context->uc_mcontext.gregs[REG_RIP] += instruct_len;
		//instruct_len = op_len((void *)(context->uc_mcontext.gregs[REG_RIP]));
		//insert_breakpoint_here((void *)(context->uc_mcontext.gregs[REG_RIP]) + instruct_len);
		debugee.step();
		usleep(10000);
		return (false);
	}
	return (true);
}

void	breakpoint_handler(Debugee &debugee) {
	//printf("Breakpoint reached:\n");

	char	*line = readline("debugger(input): ");
	while (line) {
		add_history(line);//history leaking rn on exit
		if (!handle_input(debugee, line))
			break ;
		free(line);
		line = readline("debugger: ");
	}
	free(line);
}
