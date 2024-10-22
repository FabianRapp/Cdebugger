#ifndef DEBUGGER_H
# define DEBUGGER_H

# include <stdio.h>
# include <assert.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ptrace.h>
# include <stdint.h>

# include <capstone/capstone.h>

# ifndef NOP_OPCODE
# define NOP_OPCODE 0x90
# endif //NOP_OPCODE


# ifndef BASIC_BREAK
//oppcode 0xcc : one bye instruction thus suitable as a patching instruction
//to replace an instrution in a process
# define BASIC_BREAK asm volatile ("INT3\t\n")
# endif // BASIC_BREAK

void test_op_len(void);
size_t op_len(uint8_t *op);
#endif //DEBUGGER_H
