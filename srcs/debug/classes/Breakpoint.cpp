#include <debugger.hpp>
#include <Breakpoint.hpp>
#include <Debugee.hpp>

//inserts INT3 at postion
Breakpoint::Breakpoint(t_program_ptr position, Debugee &debugee)
	: _pos(position),
	_replaced_word(debugee.get_word(position)),
	_debugee(debugee) {
	t_word	new_word;

	t_word	mask = 0xFF;
	new_word = this->get_replaced_word() & ~mask;
	new_word |= INT3_OPCODE;
	PRINT_YELLOW("old word: " << std::hex << this->get_replaced_word());
	PRINT_YELLOW("new word: " << std::hex << new_word);
	debugee.set_word(position, new_word);
}

Breakpoint::~Breakpoint(void) {
	this->_debugee.set_word(this->get_pos(), this->get_replaced_word());
	this->_debugee.set_pc(this->get_pos());
}

Breakpoint::Breakpoint(const Breakpoint &old)
	: _pos(old.get_pos()),
	_replaced_word(old.get_replaced_word()),
	_debugee(old._debugee) {
}

Breakpoint	*Breakpoint::new_bp(t_program_ptr position, Debugee &debugee) {
	return (new Breakpoint(position, debugee));
}

size_t	Breakpoint::get_pos(void) const {
	return (this->_pos);
}

Breakpoint	&Breakpoint::operator=(const Breakpoint &right) {
	assert("can not copy Breakpoints with '='" && 0);
	(void)right;
}

t_word	Breakpoint::get_replaced_word(void) const {
	return (this->_replaced_word);
}
