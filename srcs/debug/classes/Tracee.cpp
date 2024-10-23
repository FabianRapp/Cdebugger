#include <Tracee.hpp>
/*
class Tracee {
		void		rm_bp(Breakpoint bp);
};
*/

Tracee::Tracee(void) {
}

void	Tracee::set_pid(pid_t pid) {
	this->_pid = pid;
}

pid_t	Tracee::get_pid(void) const {
	return (this->_pid);
}

Tracee::Tracee(pid_t pid)
	: _pid(pid) {
	//todo: does this need options? which ars and what data type
	ptrace(PTRACE_ATTACH, this->get_pid(), 0, 0);
}

Tracee::~Tracee(void) {
	ptrace(PTRACE_DETACH, this->get_pid(), 0, 0);
}

Tracee::Tracee(const Tracee &old)
	: _pid(old._pid) {
}

Tracee	&Tracee::operator=(const Tracee &right) {
	this->_pid = right.get_pid();
	return (*this);
}

void	Tracee::cont(void) {
	if (ptrace(PTRACE_CONT, this->get_pid(), 0, 0) < 0) {
		std::cerr << "Error Tracee::cont(): PTRACE_CONT failed"
			<< std::endl;
		assert(0);
	}
}

void	Tracee::step(void) {
	if (ptrace(PTRACE_SINGLESTEP, this->get_pid(), 0, 0) < 0) {
		std::cerr << "Error Tracee::step(): PTRACE_SINGLESTEP failed"
			<< std::endl;
		assert(0);
	}
}

void	Tracee::_refresh_regs(void) {
	if (ptrace(PTRACE_GETREGS, this->get_pid(), 0, &this->_regs) < 0) {
		std::cerr << "Error Tracee::_refresh_regs(): PTRACE_GETREGS failed"
			<< std::endl;
		assert(0);
	}
}

unsigned long long int	Tracee::get_pc(void) {
	this->_refresh_regs();
	return (this->_regs.rip);
}

void	Tracee::set_pc(unsigned long long int new_pc) {
	this->_refresh_regs();
	this->_regs.rip = new_pc;
	if (ptrace(PTRACE_SETREGS, this->get_pid(), 0, &this->_regs) < 0) {
		std::cerr << "Error Tracee::set_pc(): PTRACE_SETREGS failed"
			<< std::endl;
		assert(0);
	}
}

Breakpoint	Tracee::set_bp(size_t address) {

}

