#include <debugger.hpp>

Breakpoint	*bp = NULL;

void	setup_first_breakpoint(Debugee &debugee) {
	ERRNO_CHECK;

	//debugee.step();
	//debugee.cont();


	t_program_ptr pc = debugee.get_pc();
	//pc = 0x000000003fa8;
	//pc += 0x12a;
	//pc = 0x7ffff7847a1e;
	//pc = 140737346913652;
	//pc += 106;
	printf("enserting breakpoint \n");
	bp = debugee.new_bp(pc);

	//ERRNO_CHECK;

	//printf("PRACE_cont 1\n");
	//debugee.cont();
	//ERRNO_CHECK;
	//debugee.wait();
	//pc = debugee.get_pc();
	//pc--;
	//debugee.set_pc(pc);
	//debugee.cont();
	//debugee.wait();
	//ERRNO_CHECK;
	//while (!debugee.finished()) {
	//	breakpoint_handler(debugee);
	//	ERRNO_CHECK;
	//	debugee.wait();
	//	ERRNO_CHECK;
	//}

	//ERRNO_CHECK;
}

void	fork_process(t_debugger *debugger, char **av, char **env) {
	ERRNO_CHECK;

	debugger->debugee = new Debugee(av[1], av + 1, env);
	//setup_first_breakpoint(*debugger->debugee);
}
