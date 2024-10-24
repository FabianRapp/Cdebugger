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
		personality(ADDR_NO_RANDOMIZE);
		ERRNO_CHECK;
		PRINT_YELLOW("CHILD: ptrace(PTRACE_TRACEME)");
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		//PRINT_YELLOW("raise(SIGSTOP)");
		PRINT_YELLOW("CHILD: going into execve " << path);
		execve(path, av, env);
		assert(0 && "execve failed");
	}
	ptrace(PTRACE_ATTACH, this->get_pid(), 0, 0);
	std::cout << "debugging " << path << " with pid " << this->get_pid() << std::endl;

	usleep(100000);
	//PRINT_GREEN("!!!!!!!!!!!!");
	//usleep(100000);
	//this->wait();
	//usleep(100000);
	//this->cont();
	//usleep(100000);
	//PRINT_YELLOW("!!!!!!!!!!!!");
	//usleep(100000);
	//this->wait();
	//usleep(100000);
	//PRINT_RED("!!!!!!!!!!!!");
	//usleep(100000);
	//exit(0);
	//TODO: data race with child process
	//this->_refresh_regs();

	//usleep(1000);
	//this->cont();


	//this->wait();
	//t_reg *regs = (t_reg *) (&this->_regs);
	//for (int i =0 ; i < 28; i++) {
	//	printf("reg %d : 0x%016x\n", i, (unsigned)regs[i]);
	//}
	//printf("start PC: %16llux\n", this->get_pc());
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

t_program_ptr	Debugee::get_pc(void) {
	this->_refresh_regs();
	return ((t_program_ptr)this->_regs.rip);
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
	if (!WIFSIGNALED(status) && WIFEXITED(status)) {
		this->_finished = true;
	} else if (WIFSTOPPED(status) && WSTOPSIG(status) == 11) {
		PRINT_RED("CHILD SEGFAULTED");
		this->_finished = true;
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

