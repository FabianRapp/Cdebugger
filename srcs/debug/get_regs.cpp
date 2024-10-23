#include <debugger.hpp>

void	update_regs(t_debugger *debugger) {
	printf("update regs\n");
	assert(ptrace(PTRACE_GETREGS, debugger->pid, 0, &debugger->regs) >= 0);
}


