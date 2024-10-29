#include <Debugee.hpp>

//long ptrace(enum __ptrace_request op, pid_t pid, void *addr, void *data);

void	Debugee::_parse_maps(void) {
	std::string	path = "/proc/" + std::to_string(this->_pid) + "/maps";


}

pid_t	Debugee::get_pid(void) const {
return (this->_pid);
}

Debugee::Debugee(char *path, char **av, char **env)
: _pid(fork()), _finished(false), _paused(false), _name(path),
  _memmaps(_pid) {
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
	if (this->_name[0] == '.') {
		this->_name.erase(0, 2);
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

Debugee::Debugee(pid_t pid)
 : _pid(pid), _memmaps(pid) {
	std::string	cmdline_path = "/proc/" + std::to_string(pid) + "/cmdline";

	std::ifstream	cmdline_file(cmdline_path);
	assert(cmdline_file.is_open());
	std::string		line;
	getline(cmdline_file, line);
	cmdline_file.close();
	if (line[0] == '.') {
		line.erase(0, 2);
	}
	this->_name = line;
	ERRNO_CHECK;
	printf("pid: %d\n", pid);
	ptrace(PTRACE_ATTACH, pid, 0, 0);
	//kill(pid, SIGTRAP);
	ERRNO_CHECK;
	int stat;
	waitpid(pid, &stat, 0);
	this->_paused = true;
	ERRNO_CHECK;
	PRINT_GREEN("Debugging " << pid);
}

Debugee::~Debugee(void) {
	ptrace(PTRACE_DETACH, this->get_pid(), 0, 0, 0);
}

Debugee::Debugee(const Debugee &old)
	: _pid(old._pid), _memmaps(old._pid) {
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

t_reg_index	str_to_reg(char *str) {
    if (strcmp(str, "R15") == 0)       return R15;
    else if (strcmp(str, "R14") == 0)  return R14;
    else if (strcmp(str, "R13") == 0)  return R13;
    else if (strcmp(str, "R12") == 0)  return R12;
    else if (strcmp(str, "RBP") == 0)  return RBP;
    else if (strcmp(str, "RBX") == 0)  return RBX;
    else if (strcmp(str, "R11") == 0)  return R11;
    else if (strcmp(str, "R10") == 0)  return R10;
    else if (strcmp(str, "R9") == 0)   return R9;
    else if (strcmp(str, "R8") == 0)   return R8;
    else if (strcmp(str, "RAX") == 0)  return RAX;
    else if (strcmp(str, "RCX") == 0)  return RCX;
    else if (strcmp(str, "RDX") == 0)  return RDX;
    else if (strcmp(str, "RSI") == 0)  return RSI;
    else if (strcmp(str, "RDI") == 0)  return RDI;
    else if (strcmp(str, "ORIG_RAX") == 0) return ORIG_RAX;
    else if (strcmp(str, "RIP") == 0)  return RIP;
    else if (strcmp(str, "CS") == 0)   return CS;
    else if (strcmp(str, "EFLAGS") == 0) return EFLAGS;
    else if (strcmp(str, "RSP") == 0)  return RSP;
    else if (strcmp(str, "SS") == 0)   return SS;
    else if (strcmp(str, "FS_BASE") == 0) return FS_BASE;
    else if (strcmp(str, "GS_BASE") == 0) return GS_BASE;
    else if (strcmp(str, "DS") == 0)   return DS;
    else if (strcmp(str, "ES") == 0)   return ES;
    else if (strcmp(str, "FS") == 0)   return FS;
    else if (strcmp(str, "GS") == 0)   return GS;
    else return REGS_COUNT;
}

const char	*reg_to_str(t_reg_index reg) {
	switch (reg) {
		case R15:	   return "R15";
		case R14:	   return "R14";
		case R13:	   return "R13";
		case R12:	   return "R12";
		case RBP:	   return "RBP";
		case RBX:	   return "RBX";
		case R11:	   return "R11";
		case R10:	   return "R10";
		case R9:		return "R9";
		case R8:		return "R8";
		case RAX:	   return "RAX";
		case RCX:	   return "RCX";
		case RDX:	   return "RDX";
		case RSI:	   return "RSI";
		case RDI:	   return "RDI";
		case ORIG_RAX:  return "ORIG_RAX";
		case RIP:	   return "RIP";
		case CS:		return "CS";
		case EFLAGS:	return "EFLAGS";
		case RSP:	   return "RSP";
		case SS:		return "SS";
		case FS_BASE:   return "FS_BASE";
		case GS_BASE:   return "GS_BASE";
		case DS:		return "DS";
		case ES:		return "ES";
		case FS:		return "FS";
		case GS:		return "GS";
		case REGS_COUNT:return "REGS_COUNT";
		default:		return "UNKNOWN";
	}
}

void	Debugee::set_reg(t_reg_index idx, unsigned long long val) {
	this->_refresh_regs();
	std::cout << "set reg " << reg_to_str(idx) << " to " << val << std::endl;
	((unsigned long long *)&this->_regs)[idx] = val;
	ptrace(PTRACE_SETREGS, this->_pid, 0, &this->_regs);
	this->_refresh_regs();
	ERRNO_CHECK;
}

unsigned long long	Debugee::get_reg(t_reg_index idx) {
	this->_refresh_regs();
	ERRNO_CHECK;
	return (((unsigned long long *)(&this->_regs))[idx]);
}

void	Debugee::dump_regs(void) {
	this->_refresh_regs();
	for (int i = 0; i < REGS_COUNT; i++) {
		std::cout << reg_to_str((t_reg_index) i) << ": "
			<< ((unsigned long long *)(&(this->_regs)))[i] << std::endl;
	}
}
/*
#include <sys/uio.h>
extern ssize_t process_vm_readv (pid_t __pid, const struct iovec *__lvec,
				 unsigned long int __liovcnt,
				 const struct iovec *__rvec,
				 unsigned long int __riovcnt,
				 unsigned long int __flags)
  __THROW;
*/
//todo: verifiy this
void	Debugee::read_data(t_program_ptr address, void *buffer, size_t len) {
	size_t		i = 0;
	t_word	cur_word;

	while (i < len)
	{
		cur_word = this->get_word(address + i);
		//cur_word = ptrace(PTRACE_PEEKTEXT, this->_pid, address + i, NULL);
		size_t	word_idx = 0;
		while (word_idx < sizeof (t_word) && i < len)
		{
			ERRNO_CHECK;
			((uint8_t *)buffer)[i++] = ((uint8_t *)(&cur_word))[word_idx++];
		}
	}
}
