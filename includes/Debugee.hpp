#ifndef DEBUGEE_HPP
# define DEBUGEE_HPP

# include <types.hpp>
# include <unistd.h>
# include <Breakpoint.hpp>
# include <sys/ptrace.h>
# include <sys/user.h>
# include <cassert>
# include <iostream>
# include <string>
# include <cstring>
# include <sys/wait.h>

# include <macros.h>

class Debugee {
private:
								Debugee(void);
		pid_t					_pid;
		void					set_pid(pid_t pid);
		void					_refresh_regs(void);
		struct user_regs_struct	_regs;
public:
								Debugee(pid_t pid);
								Debugee(const Debugee &old);
		Debugee					&operator=(const Debugee &right);
								~Debugee(void);
		pid_t					get_pid(void) const;
		void					cont(void);
		void					step(void);
		t_reg					get_pc(void);
		void					set_pc(t_reg new_pc);
		Breakpoint				*new_bp(t_program_ptr address);
		t_word					get_word(t_program_ptr address);
		void					set_word(t_program_ptr address, t_word word);
		void					wait(void);
};

#endif //DEBUGEE_HPP
