#include <MemMaps.hpp>

MemMaps::MemMaps(void) {
}

MemMaps::MemMaps(const MemMaps &old) {
	this->_ranges = old._ranges;
}

MemMaps	&MemMaps::operator=(const MemMaps &right) {
	this->_ranges = right._ranges;
	return (*this);
}

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
			std::cout << "entry: " << entry << std::endl;
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

void	MemMaps::print(void) const {
	for (size_t i = 0; i < this->_ranges.size(); i++) {
		t_range	cur = this->_ranges[i];
		std::cout << std::hex << cur.start << "-" << cur.end << "|"
			<< cur.read << cur.write << cur.execute << cur.shared << std::endl;
	}
}

bool	MemMaps::_check_range(t_addr address,
	enum range_check type) const {
	std::cout << "checking address " << std::hex << address << " for " << type << ":";
	for (size_t i = 0; i < this->_ranges.size(); i++) {
		if (address < this->_ranges[i].start || address >= this->_ranges[i].end)
			continue ;
		switch (type) {
			case (RANGE_ANY): {
				std::cout << "1\n";
				return (true);
			} case (PERMISSION_READ): {
				std::cout << this->_ranges[i].read << std::endl;
				return (this->_ranges[i].read);
			} case (PERMISSION_WRITE): {
				std::cout << this->_ranges[i].write << std::endl;
				return (this->_ranges[i].write);
			} case (PERMISSION_EXECUTE): {
				std::cout << this->_ranges[i].execute << std::endl;
				return (this->_ranges[i].execute);
			} case (PERMISSION_SHARED): {
				std::cout << this->_ranges[i].shared << std::endl;
				return (this->_ranges[i].shared);
			}
		}
	}
	std::cout << "address " << std::hex << address << std::dec << " does not match check " << type << std::endl;
	return (false);
}

bool	MemMaps::in_any_range(t_addr address) const {
	return (this->_check_range(address, RANGE_ANY));
}

bool	MemMaps::is_readable(t_addr address) const {
	return (this->_check_range(address, PERMISSION_READ));
}

bool	MemMaps::is_writeable(t_addr address) const {
	return (this->_check_range(address, PERMISSION_WRITE));
}

bool	MemMaps::is_executable(t_addr address) const {
	return (this->_check_range(address, PERMISSION_EXECUTE));
}

bool	MemMaps::is_shared(t_addr address) const {
	return (this->_check_range(address, PERMISSION_SHARED));
}
