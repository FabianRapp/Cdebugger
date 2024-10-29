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

class MemMaps {
private:
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

	enum permission_type {
		PERMISSION_READ,
		PERMISSION_WRITE,
		PERMISSION_EXECUTE,
		PERMISSION_SHARED,
	};

	std::vector<t_range>	_ranges;
	bool					_check_permission(unsigned long long address,
								enum permission_type type);
	void					_parse_range(t_range &cur, std::string &entry);
	void					_parse_permissions(t_range &cur, std::string &entry);
	void					_parse_path(t_range &cur, std::string &entry,
								std::ifstream &file, std::streampos &old_pos);
	void					_parse_inode(t_range &cur, std::string &entry);
	void					_parse_device(t_range &cur, std::string &entry);
	void					_parse_offset(t_range &cur, std::string &entry);
public:
							MemMaps(void) = delete;
							MemMaps(pid_t pid);
							~MemMaps(void);

			bool			is_readable(unsigned long long address);
			bool			is_writeable(unsigned long long address);
			bool			is_executable(unsigned long long address);
			bool			is_shared(unsigned long long address);
};

#endif //MEMMAPS_HPP
