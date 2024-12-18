#include <debugger.hpp>
#include <Debugee.hpp>

// return false incase the programm should continue
bool	handle_input(Debugee &debugee, char *line) {
	if (!strncmp(line, "continue", strlen("continue"))) {
		if (!bp || bp->get_pos() != debugee.get_pc()) {
			debugee.cont();
			return (false);
		}
		PRINT_GREEN("continue steped into breakpoint");
		t_addr	pos = bp->get_pos();
		debugee.set_pc(bp->get_pos());
		delete bp;
		//debugee.set_pc(pos - 1);
		(void)pos;
		bp = 0;
		debugee.cont();
		return (false);
	} else if (!strncmp(line, "REGS", strlen("REGS"))) {
		debugee.dump_regs();
	} else if (!strncmp(line, "MAPS", strlen("MAPS"))) {
		debugee.print_maps();
	} else if (!strncmp(line, "pid", strlen("pid"))) {
		std::cout << std::dec << debugee.get_pid() << std::hex << std::endl;
	} else if (!strncmp(line, "SYS", strlen("SYS"))) {
		if (!bp || bp->get_pos() != debugee.get_pc()) {
			debugee.next_syscall();
			return (false);
		}
		PRINT_GREEN("SYS steped into breakpoint");
		t_addr	pos = bp->get_pos();
		debugee.set_pc(bp->get_pos());
		delete bp;
		//debugee.set_pc(pos - 1);
		(void)pos;
		bp = 0;
		debugee.next_syscall();
		return (false);
	} else if (!strncmp(line, "SET REG", strlen("SET REG"))) {
		char	*index_str = readline("Which reg(call caps):");
		char	*new_val_str = readline("Value:");
		t_reg_index idx = str_to_reg(index_str);
		unsigned long long	val = atoi(new_val_str);
		debugee.set_reg(idx, val);
		free(index_str);
		free(new_val_str);
	} else if (!strncmp(line, "n", 1)
	    || !strncmp(line, "next", strlen("next"))) {
		t_word	cur_word = debugee.get_word(debugee.get_pc());
		if ((cur_word & 0xFF) == 0xCC) {
			PRINT_GREEN("next steped into breakpoint");
			assert(bp->get_pos() == debugee.get_pc());
			debugee.set_pc(bp->get_pos());
			delete bp;
			bp = 0;
		}
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
	assert(debugee.is_paused());
	PRINT_GREEN("PC at " << std::hex << debugee.get_pc());
	PRINT_GREEN("cur word at bp: " << std::hex << debugee.get_word(debugee.get_pc()));
	char	*line = readline("debugger(): ");
	while (line) {
		add_history(line);//history leaking rn on exit
		if (!handle_input(debugee, line))
			break ;
		free(line);
		line = readline("debugger: ");
	}
	free(line);
}
