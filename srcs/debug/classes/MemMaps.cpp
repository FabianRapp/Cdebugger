#include <MemMaps.hpp>

static void	parse_range(t_range &cur, std::string &entry) {
	std::stringstream	start;
	start << std::hex << entry.substr(0, entry.find('-'));
	start >> cur.start;
	std::stringstream	end;
	end << std::hex << entry.substr(entry.find('-') + 1, -1);
	end >> cur.end;
}

static void	parse_permissions(t_range &cur, std::string &entry) {
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

static void	parse_offset(t_range &cur, std::string &entry) {
	std::stringstream	offset;
	offset << std::hex << entry;
	offset >> cur.offset;
}

static void	parse_device(t_range &cur, std::string &entry) {
	std::stringstream	device_minor;
	device_minor << std::hex << entry.substr(0, entry.find(':'));
	device_minor >> cur.device_minor;
	std::stringstream	device_major;
	device_major << std::hex << entry.substr(entry.find(':') + 1, -1);
	device_major >> cur.device_major;
}

static void	parse_inode(t_range &cur, std::string &entry) {
	std::stringstream	inode;
	inode << entry;
	inode >> cur.inode;
}

static void	parse_path(t_range &cur, std::string &entry, std::ifstream &file,
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
	std::string		path = "/proc/" + std::to_string(pid) + "/maps";
	std::ifstream	file(path);
	if (!pid)
		return ;
	assert(pid > 0);

	assert(file.is_open());

	while (!file.eof()) {
		t_range	cur;
	
		for (int i = 0; i < 6; i++) {
			std::string	entry;
			std::streampos	old_pos = file.tellg();
			if (!(file >> entry)) {
				break ;
			}
			// todo: wtf is this
			if (entry == "(deleted)") {
				i--;
				continue ;
			}
			switch (i) {
				case(0): {
					parse_range(cur, entry);
					break ;
				} case (1): {
					parse_permissions(cur, entry);
					break ;
				} case (2): {
					parse_offset(cur, entry);
					break ;
				} case (3): {
					parse_device(cur, entry);
					break ;
				} case (4): {
					parse_inode(cur, entry);
					break ;
				} case (5): {
					parse_path(cur, entry, file, old_pos);
					break ;
				} default: {
					assert(0);
				}
			}
		}
		this->ranges.push_back(cur);
	}
	file.close();
}

MemMaps::~MemMaps(void) {
}

