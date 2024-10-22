#ifndef DEBUGGER_H
# define DEBUGGER_H

//******************BELOW ORDER MATTERS*******************
# include <stdio.h>
#define __USE_GNU
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

# ifndef ERRNO_CHECK
#  define ERRNO_CHECK do{if(errno){printf("errno: %s\n",strerror(errno));}}while(0)
# endif //ERNNO_CHECK


# ifndef NOP_OPCODE
# define NOP_OPCODE 0x90
# endif //NOP_OPCODE

# ifndef INT3_OPCODE
# define INT3_OPCODE 0xcc
# endif //INT3_OPCODE

# ifndef NOP
# define NOP asm volatile ("NOP\t\n")
# endif //NOP

# ifndef BASIC_BREAK
//oppcode 0xcc : one bye instruction thus suitable as a patching instruction
//to replace an instrution in a process
# define BASIC_BREAK asm volatile ("INT3\t\n")
# endif // BASIC_BREAK

extern uint8_t		*replaced_program_location;
extern uintptr_t	replaced_word;

typedef struct s_debugger {
	size_t					page_size;
	pid_t					pid;
	struct user_regs_struct	regs;
}	t_debugger;

void	insert_breakpoint_here(uint8_t *program, t_debugger *debugger);
//void	int3_sig_handler(int sig, siginfo_t *info, void *context);
void	breakpoint_handler(t_debugger *debugger);
void	fork_process(t_debugger *debugger, char **av, char **env);


void test_op_len(void);
size_t op_len(uint8_t *op);

#endif //DEBUGGER_H
