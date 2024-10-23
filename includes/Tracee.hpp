#ifndef TRACEE_HPP
# define TRACEE_HPP

# ifndef __USE_GNU
#  define __USE_GNU
# endif // __USE_GNU

# include <unistd.h>
# include <Breakpoint.hpp>
# include <sys/ptrace.h>
# include <sys/user.h>
# include <cassert>
# include <iostream>

class Tracee {
private:
								Tracee(void);
		pid_t					_pid;
		void					set_pid(pid_t pid);
		void					_refresh_regs(void);
		struct user_regs_struct	_regs;
public:
									Tracee(pid_t pid);
									Tracee(const Tracee &old);
		Tracee						&operator=(const Tracee &right);
									~Tracee(void);
		pid_t						get_pid(void) const;
		void						cont(void);
		void						step(void);
		unsigned long long int		get_pc(void);
		void						set_pc(unsigned long long int new_pc);
		Breakpoint					set_bp(size_t address);
		void						rm_bp(Breakpoint bp);
};

#endif //TRACEE_HPP
