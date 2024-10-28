#include <MemMaps.hpp>

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

			if (entry == "(deleted)") {
				i--;
				continue ;
			}
			std::cout << i << ": " << entry << std::endl;
			switch (i) {
				case(0): {
					std::stringstream	start;
					start << std::hex << entry.substr(0, entry.find('-'));
					start >> cur.start;
					std::stringstream	end;
					end << std::hex << entry.substr(entry.find('-') + 1, -1);
					end >> cur.end;
					break ;
				}
				case (1): {
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
					break ;
				}
				case (2): {
					std::stringstream	offset;
					offset << std::hex << entry;
					offset >> cur.offset;
					break ;
				}
				case (3): {
					std::stringstream	device_minor;
					device_minor << std::hex << entry.substr(0, entry.find(':'));
					device_minor >> cur.device_minor;
					std::stringstream	device_major;
					device_major << std::hex << entry.substr(entry.find(':') + 1, -1);
					device_major >> cur.device_major;
					break ;
				}
				case (4): {
					std::stringstream	inode;
					inode << entry;
					inode >> cur.inode;
					break ;
				}
				case (5): {
					if (entry[0] != '/' && entry[0] != '[') {
						cur.path = "";
						file.seekg(old_pos);
						break ;
					}
					cur.path = entry;
					break ;
				}
				default: assert(0);
			}
		}
		this->ranges.push_back(cur);
	}
	file.close();
}

MemMaps::~MemMaps(void) {
}

