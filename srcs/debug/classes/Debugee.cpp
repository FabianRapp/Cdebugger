#include <Debugee.hpp>

//long ptrace(enum __ptrace_request op, pid_t pid, void *addr, void *data);

pid_t	Debugee::get_pid(void) const {
return (this->_pid);
}

Debugee::Debugee(char *path, char **av, char **env)
: _pid(fork()), _finished(false), _paused(false) {
	assert("fork failed" && this->get_pid() >= 0);
	if (this->_pid == 0) {
		personality(ADDR_NO_RANDOMIZE);
		ERRNO_CHECK;
		PRINT_YELLOW("CHILD: ptrace(PTRACE_TRACEME)");
		long ret = ptrace(PTRACE_TRACEME, 0, NULL, NULL, 0);
		assert(ret == 0);
		ERRNO_CHECK;
		PRINT_YELLOW("CHILD: going into execve " << path);
		execve(path, av, env);//causes a sigtrap so the parent can catch up
		assert(0 && "execve failed");
	}
	/* if both the child call TRACEME and the parent call ATTACH one will fail:
	ptrace(PTRACE_ATTACH, this->_pid, 0, 0, 0); */
	std::cout << "debugging " << path << " with pid " << this->_pid << std::endl;
	this->wait();//wait for execve call from child
	PRINT_GREEN("PARENT: child called execve");
	//while (!this->_finished) {
	//	ptrace(PTRACE_SYSCALL, this->_pid, 0, 0);
	//	this->wait();
	//	PRINT_YELLOW(this->get_pc());
	//	usleep(100000);
	//}
	ERRNO_CHECK;
	
}

Debugee::~Debugee(void) {
	ptrace(PTRACE_DETACH, this->get_pid(), 0, 0, 0);
}

Debugee::Debugee(const Debugee &old)
	: _pid(old._pid) {
}

Debugee	&Debugee::operator=(const Debugee &right) {
	this->_pid = right.get_pid();
	return (*this);
}

void	Debugee::cont(void) {
	assert(this->_paused);
	if (ptrace(PTRACE_CONT, this->get_pid(), 0, 0) < 0) {
		std::cerr << "Error Debugee::cont(): PTRACE_CONT failed"
			<< std::endl;
		assert(0);
	}
	ERRNO_CHECK;
	this->_paused = false;
}

void	Debugee::step(void) {
	assert(this->_paused);
	if (ptrace(PTRACE_SINGLESTEP, this->get_pid(), 0, 0) < 0) {
		std::cerr << "Error Debugee::step(): PTRACE_SINGLESTEP failed"
			<< std::endl;
		assert(0);
	}
	ERRNO_CHECK;
	this->_paused = false;
}

void	Debugee::_refresh_regs(void) {
	assert(this->_paused);
	if (ptrace(PTRACE_GETREGS, this->get_pid(), 0, &this->_regs) < 0) {
		std::cerr << "Error Debugee::_refresh_regs(): PTRACE_GETREGS failed"
			<< std::endl;
		assert(0);
	}
}

t_program_ptr	Debugee::get_pc(void) {
	assert(this->_paused);
	this->_refresh_regs();
	return ((t_program_ptr)this->_regs.rip);
}

void	Debugee::set_pc(t_reg new_pc) {
	assert(this->_paused);
	this->_refresh_regs();
	this->_regs.rip = new_pc;
	if (ptrace(PTRACE_SETREGS, this->get_pid(), 0, &this->_regs) < 0) {
		std::cerr << "Error Debugee::set_pc(): PTRACE_SETREGS failed"
			<< std::endl;
		assert(0);
	}
}

// todo: mange breakpoints in class
Breakpoint	*Debugee::new_bp(t_program_ptr address) {
	assert(this->_paused);
	return (Breakpoint::new_bp(address, *this));
}

t_word	Debugee::get_word(t_program_ptr address) {
	t_word	word;

	assert(this->_paused);
	ERRNO_CHECK;
	errno = 0;
	std::cout << "bp address: " << std::hex << address << std::endl;
	long	bytes = ptrace(PTRACE_PEEKTEXT, this->get_pid(), address, 0);
	uint8_t	b1 = bytes & 0xff;
	uint8_t	b2 = bytes & 0xff00;
	(void)b1;
	(void)b2;
	word = bytes;
	if (errno) {
		std::cerr << "Error Debugee::get_word(): PEEKTEXT failed: "
			<< strerror(errno) << std::endl;
		assert(0);
	}
	return (word);
}

void	Debugee::set_word(t_program_ptr address, t_word word) {
	assert(this->_paused);
	PRINT_YELLOW("at " << std::hex << address << ": replacing" << this->get_word(address) << " with " << word);
	if (ptrace(PTRACE_POKETEXT, this->get_pid(), address, word) < 0) {
		std::cerr << "Error Debugee::set_word(): PTRACE_POKETEXT failed"
			<< std::endl;
		assert(0);
	}
}

void wait_print_exit_status(int status) {
	if (WIFSIGNALED(status)) {
		PRINT_YELLOW("Exited due to uncaught signal: " << WEXITSTATUS(status));
	} else if (WIFEXITED(status)) {
		PRINT_YELLOW("Exited normally with exit code " << WIFEXITED(status));
	} else if (WIFSTOPPED(status)) {
		int	sig = WSTOPSIG(status);
		if (sig == 5) {
			PRINT_GREEN("Stopped by signal: " << strsignal(sig));
		} else if (sig == 11) {
			PRINT_RED("Stopped by signal: " << strsignal(sig));
		} else if (sig == 19) {
			PRINT_RED("waited proces had sig continue (19)");
		} else {
			PRINT_YELLOW("Stopped by signal: " << strsignal(sig) << "(" << sig << ")");
		}
	} else if (WIFCONTINUED(status)) {
		PRINT_YELLOW("Continued");
	} else {
		PRINT_YELLOW("Exited with unknown status");
	}
}

void	Debugee::wait(void) {
	int	status;

	waitpid(this->get_pid(), &status, 0);
	if (WIFSTOPPED(status)) {
		this->_paused = true;
		this->_last_sig = WSTOPSIG(status);
#ifndef NODEBUG
		PRINT_YELLOW("PARENT: child paused duo to sig " << strsignal(this->_last_sig));
#endif //NODEBUG
	} else {
		this->_paused = false;
	}
	if (!WIFSIGNALED(status) && WIFEXITED(status)) {
		this->_finished = true;
	} else if (WIFSTOPPED(status) && WSTOPSIG(status) == 11) {
		PRINT_RED("CHILD SEGFAULTED");
		this->_finished = true;
		this->_last_sig = 11;
	}
	wait_print_exit_status(status);
	ERRNO_CHECK;
}

bool	Debugee::finished(void) {
	return (this->_finished);
}

bool	Debugee::blocked() {
	int status;

	waitpid(this->get_pid(), &status, WNOHANG | WUNTRACED);
	if (WIFSTOPPED(status))
		return (true);
	ERRNO_CHECK;
	return (false);
}

