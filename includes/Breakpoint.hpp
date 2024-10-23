#ifndef BREAKPOINT_HPP
# define BREAKPOINT_HPP

# include <cstdlib>

class Tracee;

typedef size_t	t_word;
typedef size_t	t_program_ptr;


class Breakpoint {
private:
	const	t_program_ptr	_pos;
	const	t_word			_replaced_word;
							Breakpoint(void);
public:
							Breakpoint(t_program_ptr position, Tracee debugee);
							Breakpoint(const Breakpoint &old);
			Breakpoint		&operator=(const Breakpoint &right);
							~Breakpoint(void);
			t_program_ptr	get_pos(void) const;
			t_word			get_replaced_word(void) const;

};

#endif // BREAKPOINT_HPP
