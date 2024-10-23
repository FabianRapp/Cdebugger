#ifndef BREAKPOINT_HPP
# define BREAKPOINT_HPP

# include <types.hpp>
# include <cstdlib>

class Breakpoint {
private:
	const	t_program_ptr	_pos;
	const	t_word			_replaced_word;
			Debugee			&_debugee;
			Breakpoint		&operator=(const Breakpoint &right);
							Breakpoint(t_program_ptr position, Debugee &debugee);
public:
	static Breakpoint		*new_bp(t_program_ptr position, Debugee &debugee);
							~Breakpoint(void);
							Breakpoint(const Breakpoint &old);
			t_program_ptr	get_pos(void) const;
			t_word			get_replaced_word(void) const;

};

#endif // BREAKPOINT_HPP
