#include <MemMaps.hpp>

void	MemMaps::_parse_range(t_range &cur, std::string &entry) {
	std::stringstream	start;
	start << std::hex << entry.substr(0, entry.find('-'));
	start >> cur.start;
	std::stringstream	end;
	end << std::hex << entry.substr(entry.find('-') + 1, -1);
	end >> cur.end;
}

void	MemMaps::_parse_permissions(t_range &cur, std::string &entry) {
	if (entry[0] == '-') {
		cur.read = false;
	} else {
		assert(entry[0] == 'r');
		cur.read = true;
	}
	if (entry[1] == '-') {
		cur.write = false;
	} else {
		assert(entry[1] == 'w');
		cur.write = true;
	}
	if (entry[2] == '-') {
		cur.execute = false;
	} else {
		assert(entry[2] == 'x');
		cur.execute = true;
	}
	if (entry[3] == 'p') {
		cur.shared = false;
	} else {
		assert(entry[4] == 's');
		cur.shared = true;
	}
}

void	MemMaps::_parse_offset(t_range &cur, std::string &entry) {
	std::stringstream	offset;
	offset << std::hex << entry;
	offset >> cur.offset;
}

void	MemMaps::_parse_device(t_range &cur, std::string &entry) {
	std::stringstream	device_minor;
	device_minor << std::hex << entry.substr(0, entry.find(':'));
	device_minor >> cur.device_minor;
	std::stringstream	device_major;
	device_major << std::hex << entry.substr(entry.find(':') + 1, -1);
	device_major >> cur.device_major;
}

void	MemMaps::_parse_inode(t_range &cur, std::string &entry) {
	std::stringstream	inode;
	inode << entry;
	inode >> cur.inode;
}

void	MemMaps::_parse_path(t_range &cur, std::string &entry, std::ifstream &file,
	std::streampos &old_pos) {
	// if no path entry
	if (entry[0] != '/' && entry[0] != '[') {
		cur.path = "";
		file.seekg(old_pos);
	} else {
		cur.path = entry;
	}
}

MemMaps::MemMaps(pid_t pid) {
	if (!pid)
		return ;
	std::string		path = "/proc/" + std::to_string(pid) + "/maps";
	std::ifstream	file(path);
	assert(pid > 0);
	assert(file.is_open());

	while (!file.eof()) {
		t_range	cur;
	
		for (int col = 0; col < 6; col++) {
			std::string	entry;
			std::streampos	old_pos = file.tellg();
			if (!(file >> entry)) {
				break ;
			}
			// todo: wtf is this
			if (entry == "(deleted)") {
				col--;
				continue ;
			}
			switch (col) {
				case(0): {
					this->_parse_range(cur, entry);
					break ;
				} case (1): {
					this->_parse_permissions(cur, entry);
					break ;
				} case (2): {
					this->_parse_offset(cur, entry);
					break ;
				} case (3): {
					this->_parse_device(cur, entry);
					break ;
				} case (4): {
					this->_parse_inode(cur, entry);
					break ;
				} case (5): {
					this->_parse_path(cur, entry, file, old_pos);
					break ;
				} default: {
					assert(0);
				}
			}
		}
		this->_ranges.push_back(cur);
	}
	file.close();
	ERRNO_CHECK;
}

MemMaps::~MemMaps(void) {
}

bool	MemMaps::_check_permission(unsigned long long address,
	enum permission_type type) {
	for (size_t i = 0; i < this->_ranges.size(); i++) {
		if (address < this->_ranges[i].start || address >= this->_ranges[i].end)
			continue ;
		switch (type) {
			case (PERMISSION_READ): return (this->_ranges[i].read);
			case (PERMISSION_WRITE): return (this->_ranges[i].write);
			case (PERMISSION_EXECUTE): return (this->_ranges[i].execute);
			case (PERMISSION_SHARED): return (this->_ranges[i].shared);
		}
	}
	return (false);
}

bool	MemMaps::is_readable(unsigned long long address) {
	return (this->_check_permission(address, PERMISSION_READ));
}

bool	MemMaps::is_writeable(unsigned long long address) {
	return (this->_check_permission(address, PERMISSION_WRITE));
}

bool	MemMaps::is_executable(unsigned long long address) {
	return (this->_check_permission(address, PERMISSION_EXECUTE));
}

bool	MemMaps::is_shared(unsigned long long address) {
	return (this->_check_permission(address, PERMISSION_SHARED));
}
