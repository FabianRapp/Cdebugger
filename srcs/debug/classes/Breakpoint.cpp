#include <debugger.hpp>
#include <Breakpoint.hpp>
#include <Tracee.hpp>

Breakpoint::Breakpoint(void)
	: _pos(0) , _replaced_word(0){
	assert("Breakpoint has to be constructed with pos and debugee or "
		"with a copy" && 0);
}

//TODO:
Breakpoint::Breakpoint(t_program_ptr position, Tracee debugee)
	: _pos(position), _replaced_word(0) {
}

Breakpoint::Breakpoint(const Breakpoint &old)
	: _pos(old.get_pos()), _replaced_word(old.get_replaced_word()) {
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
