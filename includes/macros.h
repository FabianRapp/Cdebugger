#ifndef MACROS_H
# define MACROS_H
# include <string.h>

# ifndef	PRINT_RED
#  define PRINT_RED(content) std::cout << "\033[31m" << content << "\033[0m" << std::endl;
# endif //PRINT_RED

# ifndef PRINT_YELLOW
#  define PRINT_YELLOW(content) std::cout << "\033[33m" << content << "\033[0m" << std::endl;
# endif // PRINT_YELLOW

# ifndef PRINT_GREEN
#  define PRINT_GREEN(content) std::cout << "\033[32m" << content << "\033[0m" << std::endl;
# endif // PRINT_GREEN

# ifndef ERRNO_CHECK
#  define ERRNO_CHECK do {if (errno){ \
	printf("errno: %d: %s(%s line %d)\n", errno, strerror(errno), __FILE__, __LINE__); \
	assert(errno == 0);}} while(0)
# endif //ERNNO_CHECK

# ifndef NOP_OPCODE
#  define NOP_OPCODE 0x90
# endif //NOP_OPCODE

# ifndef INT3_OPCODE
#  define INT3_OPCODE 0xcc
# endif //INT3_OPCODE

# ifndef NOP
#  define NOP asm volatile ("NOP\t\n")
# endif //NOP

# ifndef BASIC_BREAK
//oppcode 0xcc : one bye instruction thus suitable as a patching instruction
//to replace an instrution in a process
#  define BASIC_BREAK asm volatile ("INT3\t\n")
# endif // BASIC_BREAK

#endif //MACROS_H
