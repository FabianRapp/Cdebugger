#ifndef DEBUGEE_HPP
# define DEBUGEE_HPP

# include <types.hpp>
# include <unistd.h>
# include <Breakpoint.hpp>
# include <sys/ptrace.h>
# include <sys/user.h>
# include <cassert>
# include <iostream>
# include <fstream>
# include <string>
# include <cstring>
# include <sys/wait.h>
# include <sys/personality.h>
# include <string>
# include <MemMaps.hpp>

# include <macros.h>
//****forward declatrions
void	block_signals(void);
void	unblock_signals(void);
//*********************************
typedef enum e_reg_index {
	R15 = 0,
	R14,
	R13,
	R12,
	RBP,
	RBX,
	R11,
	R10,
	R9,
	R8,
	RAX,
	RCX,
	RDX,
	RSI,
	RDI,
	ORIG_RAX,
	RIP,
	CS,
	EFLAGS,
	RSP,
	SS,
	FS_BASE,
	GS_BASE,
	DS,
	ES,
	FS,
	GS,
	REGS_COUNT
} t_reg_index;


const char	*reg_to_str(t_reg_index reg);
t_reg_index	str_to_reg(char *str);

//todo: does constructor need options for PTRACE_ATTACH?
class Debugee {
private:
		pid_t					_pid;
		void					_refresh_regs(void);
		void					_refresh_maps(void);
		struct user_regs_struct	_regs;
		bool					_finished;
		bool					_paused;
		int						_last_sig;
		std::string				_name;
		MemMaps					_memmaps;
public:
								Debugee(void) = delete;
								Debugee(char *path, char **av, char **env);
								Debugee(pid_t pid);
								Debugee(const Debugee &old);
		Debugee					&operator=(const Debugee &right);
								~Debugee(void);
		bool					is_paused(void) const;
		pid_t					get_pid(void) const;
		void					cont(void);
		void					next_syscall(void);
		void					step(void);
		t_addr					get_pc(void);
		void					set_pc(t_reg new_pc);
		Breakpoint				*new_bp(t_addr address);
		t_word					get_word(t_addr address);
		void					set_word(t_addr address, t_word word);
		void					wait(void);
		bool					finished(void);
		bool					blocked(void);
		void					dump_regs(void);
		void					set_reg(t_reg_index idx, unsigned long long val);
		unsigned long long		get_reg(t_reg_index idx);
		void					read_data(t_addr address, void *buffer, size_t len);
		void					print_maps(void) const;
};

#endif //DEBUGEE_HPP
