#ifndef DEBUGGER1_H
# define DEBUGGER1_H

//******************BELOW ORDER MATTERS*******************
# include <stdio.h>
//# ifndef __USE_GNU
//#  define __USE_GNU
//# endif // __USE_GNU
//******************ABOVE ORDER MATTERS*******************
# include <signal.h>
# include <sys/ucontext.h>

# include <ucontext.h>
# include <assert.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ptrace.h>
# include <stdint.h>
# include <unistd.h>
# include <sys/mman.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <sys/user.h>

# include <capstone/capstone.h>

# include <string>
# include <iostream>
# include <macros.h>

# include <Debugee.hpp>



extern uint8_t		*replaced_program_location;
extern uint64_t		replaced_word;
extern Breakpoint	*bp;

typedef struct s_debugger {
	Debugee					*debugee;
	size_t					page_size;
	pid_t					pid;
	struct user_regs_struct	regs;
}	t_debugger;
extern t_debugger	debugger;

void	insert_breakpoint_here(uint8_t *program, t_debugger *debugger);
//void	int3_sig_handler(int sig, siginfo_t *info, void *context);
void	breakpoint_handler(Debugee &debugee);
void	fork_process(t_debugger *debugger, char **av, char **env);


void	test_op_len(void);
size_t	op_len(t_program_ptr op);

void	check_child_status(pid_t child_pid);

void	remove_cur_breakpoint(t_debugger *debugger);
#endif //DEBUGGER1_H
