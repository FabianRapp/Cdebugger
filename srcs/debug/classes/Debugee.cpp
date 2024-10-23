#include <Debugee.hpp>

Debugee::Debugee(void) {
}

void	Debugee::set_pid(pid_t pid) {
	this->_pid = pid;
}

pid_t	Debugee::get_pid(void) const {
	return (this->_pid);
}

Debugee::Debugee(pid_t pid)
	: _pid(pid) {
	//todo: does this need options? which ars and what data type
	ptrace(PTRACE_ATTACH, this->get_pid(), 0, 0);
}

Debugee::~Debugee(void) {
	ptrace(PTRACE_DETACH, this->get_pid(), 0, 0);
}

Debugee::Debugee(const Debugee &old)
	: _pid(old._pid) {
}

Debugee	&Debugee::operator=(const Debugee &right) {
	this->_pid = right.get_pid();
	return (*this);
}

void	Debugee::cont(void) {
	if (ptrace(PTRACE_CONT, this->get_pid(), 0, 0) < 0) {
		std::cerr << "Error Debugee::cont(): PTRACE_CONT failed"
			<< std::endl;
		assert(0);
	}
}

void	Debugee::step(void) {
	if (ptrace(PTRACE_SINGLESTEP, this->get_pid(), 0, 0) < 0) {
		std::cerr << "Error Debugee::step(): PTRACE_SINGLESTEP failed"
			<< std::endl;
		assert(0);
	}
}

void	Debugee::_refresh_regs(void) {
	if (ptrace(PTRACE_GETREGS, this->get_pid(), 0, &this->_regs) < 0) {
		std::cerr << "Error Debugee::_refresh_regs(): PTRACE_GETREGS failed"
			<< std::endl;
		assert(0);
	}
}

t_reg	Debugee::get_pc(void) {
	this->_refresh_regs();
	return (this->_regs.rip);
}

void	Debugee::set_pc(t_reg new_pc) {
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
	return (Breakpoint::new_bp(address, *this));
}

t_word	Debugee::get_word(t_program_ptr address) {
	t_word	word;

	ERRNO_CHECK;
	errno = 0;
	word = ptrace(PTRACE_PEEKTEXT, this->get_pid(), address, 0);
	if (errno) {
		std::cerr << "Error Debugee::get_word(): PEEKTEXT failed: "
			<< strerror(errno) << std::endl;
		assert(0);
	}
	return (word);
}

void	Debugee::set_word(t_program_ptr address, t_word word) {
	if (ptrace(PTRACE_POKETEXT, this->get_pid(), address, word) < 0) {
		std::cerr << "Error Debugee::set_word(): PTRACE_POKETEXT failed"
			<< std::endl;
		assert(0);
	}
}

void wait_print_exit_status(int status) {
	if (WIFSIGNALED(status)) {
		printf("Exited due to uncaught signal: %d\n", WTERMSIG(status));
	} else if (WIFEXITED(status)) {
		printf("Exited normally with exit code %d\n", WEXITSTATUS(status));
	} else if (WIFSTOPPED(status)) {
		printf("Stopped by signal: %d\n", WSTOPSIG(status));
	} else if (WIFCONTINUED(status)) {
		printf("Continued\n");
	} else {
		printf("Exited with unknown status\n");
	}
}

void	Debugee::wait(void) {
	int	status;

	waitpid(this->get_pid(), &status, 0);
	wait_print_exit_status(status);
}
