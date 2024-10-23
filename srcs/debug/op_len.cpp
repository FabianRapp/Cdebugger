#include <debugger.hpp>

size_t op_len(t_program_ptr op) {
	csh handle;
	cs_insn *insn;
	size_t size = 0;

	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) == CS_ERR_OK) {
		if (cs_disasm(handle, (uint8_t *)op, 15, (uintptr_t)op, 1, &insn) > 0) {
			size = insn[0].size;
			cs_free(insn, 1);
		}
		cs_close(&handle);
	} else {assert(CS_ERR_OK && 0);}
	return size;
}

void test_op_len(void) {
	unsigned char code[] = {
		0xCC,								// int3
		0xB8, 0x01, 0x00, 0x00, 0x00,		// mov eax, 1
		0xC3,								// ret
		0xCC,								// int3
		0x90,								// nop
		0x00, 0x00,							// add byte ptr [eax], al
		0xFF, 0xC1,							// inc ecx (one-byte opcode)
		0x53,								// push ebx
		0x5B,								// pop ebx
		0x0F, 0xB6, 0x08					 // movzx ecx, byte ptr [eax]
	};

	size_t len[30] = {0};
	t_program_ptr ptr = (t_program_ptr)code;

	// int3
	len[0] = op_len(ptr);
	ptr += len[0];
	assert(len[0] == 1);

	// mov eax, 1
	len[1] = op_len(ptr);
	ptr += len[1];
	assert(len[1] == 5);

	// ret
	len[2] = op_len(ptr);
	ptr += len[2];
	assert(len[2] == 1);

	// int3
	len[3] = op_len(ptr);
	ptr += len[3];
	assert(len[3] == 1);

	// nop
	len[4] = op_len(ptr);
	ptr += len[4];
	assert(len[4] == 1);

	// add byte ptr [eax], al
	len[5] = op_len(ptr);
	ptr += len[5];
	assert(len[5] == 2);

	// inc ecx
	len[6] = op_len(ptr);
	ptr += len[6];
	assert(len[6] == 2);

	// push ebx
	len[7] = op_len(ptr);
	ptr += len[7];
	assert(len[7] == 1);

	// pop ebx
	len[8] = op_len(ptr);
	ptr += len[8];
	assert(len[8] == 1);

	// movzx ecx, byte ptr [eax]
	len[9] = op_len(ptr);
	ptr += len[9];
	assert(len[9] == 3);

	printf("all op_len tests passed\n");
}
