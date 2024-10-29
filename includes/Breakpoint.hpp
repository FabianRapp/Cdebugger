#ifndef BREAKPOINT_HPP
# define BREAKPOINT_HPP

# include <types.hpp>
# include <cstdlib>

class Breakpoint {
private:
	const	t_addr			_pos;
	const	t_word			_replaced_word;
			Debugee			&_debugee;
			Breakpoint		&operator=(const Breakpoint &right);
							Breakpoint(t_addr position, Debugee &debugee);
public:
	static Breakpoint		*new_bp(t_addr position, Debugee &debugee);
							~Breakpoint(void);
							Breakpoint(const Breakpoint &old);
			t_addr			get_pos(void) const;
			t_word			get_replaced_word(void) const;

};

#endif // BREAKPOINT_HPP
