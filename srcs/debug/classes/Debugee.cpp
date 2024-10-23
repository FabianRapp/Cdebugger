#include <Debugee.hpp>

Debugee::Debugee(void) {
}

pid_t	Debugee::get_pid(void) const {
	return (this->_pid);
}

Debugee::Debugee(char *path, char **av, char **env)
	: _pid(fork()), _finished(false) {
	assert("fork failed" && this->get_pid() >= 0);
	if (this->get_pid() == 0) {
		ERRNO_CHECK;
		//PRINT_YELLOW("ptrace(PTRACE_TRACEME)");
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		//PRINT_YELLOW("raise(SIGSTOP)");
		//PRINT_YELLOW("execve %s" << av[1]);
		execve(path, av + 1, env);
		assert(0 && "execve failed");
	}
	PRINT_RED("TEST");
	printf("?!\n");
	std::cout.flush();
	ptrace(PTRACE_ATTACH, this->get_pid(), 0, 0);
	std::cout.flush();
	PRINT_GREEN("debugging " << av[1] << " with pid " << this->get_pid());

	PRINT_RED("TEST");
	//TODO: data race with child process
	usleep(1000);
	//this->wait();
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
		PRINT_RED("Exited due to uncaught signal: " << WEXITSTATUS(status));
	} else if (WIFEXITED(status)) {
		PRINT_RED("Exited normally with exit code " << WIFEXITED(status));
	} else if (WIFSTOPPED(status)) {
		PRINT_RED("Stopped by signal: " << WSTOPSIG(status));
	} else if (WIFCONTINUED(status)) {
		PRINT_RED("Continued");
	} else {
		PRINT_RED("Exited with unknown status");
	}
}

void	Debugee::wait(void) {
	int	status;

	if (waitpid(this->get_pid(), &status, 0) < 0)
	{
		if (WIFEXITED(status))
			this->_finished = true;
	}
	wait_print_exit_status(status);
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

