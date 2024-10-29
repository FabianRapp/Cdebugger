#ifndef MEMMAPS_HPP
# define MEMMAPS_HPP

# include <unistd.h>
# include <string>
# include <fstream>
# include <iostream>
# include <sstream>
# include <cassert>
# include <vector>
# include <macros.h>
# include <types.hpp>

class MemMaps {
private:
	typedef struct s_range {
		t_addr		start;
		t_addr		end;
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

	enum range_check {
		RANGE_ANY,
		PERMISSION_READ,
		PERMISSION_WRITE,
		PERMISSION_EXECUTE,
		PERMISSION_SHARED,
	};

	std::vector<t_range>	_ranges;
	bool					_check_range(t_addr address,
								enum range_check type) const;
	void					_parse_range(t_range &cur, std::string &entry);
	void					_parse_permissions(t_range &cur, std::string &entry);
	void					_parse_path(t_range &cur, std::string &entry,
								std::ifstream &file, std::streampos &old_pos);
	void					_parse_inode(t_range &cur, std::string &entry);
	void					_parse_device(t_range &cur, std::string &entry);
	void					_parse_offset(t_range &cur, std::string &entry);
public:
							MemMaps(void);
							MemMaps(pid_t pid);
							~MemMaps(void);
							MemMaps(const MemMaps &old);
			MemMaps			&operator=(const MemMaps &right);

			void			print(void) const;

			bool			in_any_range(t_addr address) const;
			bool			is_readable(t_addr address) const;
			bool			is_writeable(t_addr address) const;
			bool			is_executable(t_addr address) const;
			bool			is_shared(t_addr address) const;
};

#endif //MEMMAPS_HPP
