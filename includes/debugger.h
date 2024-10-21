#ifndef DEBUGGER_H
# define DEBUGGER_H

# include <stdio.h>
# include <assert.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ptrace.h>


# ifndef BASIC_BREAK
# define BASIC_BREAK asm volatile ("INT3\t\n")
# endif // BASIC_BREAK


#endif //DEBUGGER_H
