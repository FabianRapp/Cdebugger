#ifndef MEMMAPS_HPP
# define MEMMAPS_HPP

# include <unistd.h>
# include <string>
# include <fstream>
# include <iostream>
# include <sstream>
# include <cassert>
# include <vector>

typedef struct s_range {
	size_t		start;
	size_t		end;
	bool		read;
	bool		write;
	bool		execute;
	bool		shared;
	size_t		offset;
	size_t		device_minor;
	size_t		device_major;
	size_t		inode;
	std::string	path;
}	t_range;

class MemMaps {
public:
	std::vector<t_range>	ranges;
							MemMaps(void) = delete;
							MemMaps(pid_t pid);
							~MemMaps(void);
};

#endif //MEMMAPS_HPP
